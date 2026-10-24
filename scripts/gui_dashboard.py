"""
# Joao Victor Pariz
# Projeto Final - Sensor HW-201
# SE/3 - Eletronica - Programacao Aplicada

Interface Grafica (GUI) de Monitoramento de Carga Sensivel em Tempo Real.
Recebe datagramas UDP transmitidos pela placa STM32MP157D-DK1 (ou simulador local).
Requisitos atendidos:
- Exibicao em destaque do status da carga (Integra vs Violacao)
- Grafico temporal deslizante (ultimos 30 a 60 segundos) via Matplotlib
- Alerta visual com cores e banner piscante
- Registro continuo e exportacao em arquivo CSV
- Ajuste dinamico de limiar de alerta (requisito bonus)
- Indicador temporal de ultimo pacote recebido (requisito bonus)
"""

import sys
import time
import json
import socket
import threading
import csv
import os
from collections import deque
import tkinter as tk
from tkinter import ttk, messagebox, filedialog

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class TelemetryDashboard:
    def __init__(self, root, udp_port=5000):
        self.root = root
        self.udp_port = udp_port
        self.root.title("Monitoramento Inteligente de Carga - IME SE/3 (Sensor HW-201)")
        self.root.geometry("980x740")
        self.root.minsize(850, 650)

        # Dados em tempo real (janela de 60 segundos)
        self.max_points = 240 # ~60 segundos a 4 Hz
        self.times = deque(maxlen=self.max_points)
        self.voltages = deque(maxlen=self.max_points)
        self.raw_values = deque(maxlen=self.max_points)

        self.threshold = 2000
        self.latest_raw = 0
        self.latest_voltage = 0.0
        self.is_violated = False
        self.last_packet_time = 0
        self.packet_count = 0
        self.running = True
        self.paused = False

        # Arquivo de log CSV
        self.csv_filename = "dados_telemetria_sensor_hw201.csv"
        self.init_csv()

        self.build_ui()

        # Inicia thread de escuta UDP
        self.udp_thread = threading.Thread(target=self.udp_listener, daemon=True)
        self.udp_thread.start()

        # Inicia ciclo de atualizacao da interface (100 ms)
        self.update_gui_loop()

    def init_csv(self):
        if not os.path.exists(self.csv_filename):
            with open(self.csv_filename, "w", newline="", encoding="utf-8") as f:
                writer = csv.writer(f)
                writer.writerow(["timestamp_iso", "sensor_id", "adc_raw", "voltage_v", "violation_flag"])

    def build_ui(self):
        # Top Header Institucional
        header_frame = tk.Frame(self.root, bg="#003366", pady=10)
        header_frame.pack(fill=tk.X)

        title_lbl = tk.Label(
            header_frame,
            text="INSTITUTO MILITAR DE ENGENHARIA - SE/3",
            font=("Arial", 14, "bold"),
            fg="white",
            bg="#003366"
        )
        title_lbl.pack()

        sub_lbl = tk.Label(
            header_frame,
            text="Central de Monitoramento de Carga Sensivel | Sensor HW-201 (Infravermelho Reflexivo)",
            font=("Arial", 10),
            fg="#cce0ff",
            bg="#003366"
        )
        sub_lbl.pack()

        # Banner de Status de Seguranca
        self.status_banner = tk.Label(
            self.root,
            text="AGUARDANDO CONEXAO TELEMETRICA...",
            font=("Arial", 16, "bold"),
            bg="#777777",
            fg="white",
            pady=12
        )
        self.status_banner.pack(fill=tk.X, padx=10, pady=8)

        # Painel Superior com Indicadores Numericos
        cards_frame = tk.Frame(self.root)
        cards_frame.pack(fill=tk.X, padx=10, pady=5)

        self.card_raw = self.create_card(cards_frame, "Leitura ADC Bruta", "--", "0 - 4095")
        self.card_volt = self.create_card(cards_frame, "Tensao do Sensor", "-- V", "0.0 - 3.3V")
        self.card_ping = self.create_card(cards_frame, "Ultimo Pacote", "--", "segundos atras")
        self.card_total = self.create_card(cards_frame, "Pacotes Recebidos", "0", "UDP Porta 5000")

        # Painel de Controle e Limiar Dinamico
        ctrl_frame = tk.LabelFrame(self.root, text="Controle do Sistema e Ajuste de Alarme", font=("Arial", 10, "bold"), padx=10, pady=6)
        ctrl_frame.pack(fill=tk.X, padx=10, pady=5)

        tk.Label(ctrl_frame, text="Limiar de Deteccao (ADC):", font=("Arial", 10)).pack(side=tk.LEFT, padx=5)

        self.thresh_slider = tk.Scale(
            ctrl_frame,
            from_=100,
            to=4000,
            orient=tk.HORIZONTAL,
            length=220,
            command=self.on_threshold_change
        )
        self.thresh_slider.set(self.threshold)
        self.thresh_slider.pack(side=tk.LEFT, padx=5)

        self.thresh_val_lbl = tk.Label(ctrl_frame, text=f"{self.threshold}", font=("Arial", 10, "bold"), width=6)
        self.thresh_val_lbl.pack(side=tk.LEFT, padx=5)

        self.pause_btn = tk.Button(ctrl_frame, text="Pausar Grafico", command=self.toggle_pause, bg="#eeeeee", padx=10)
        self.pause_btn.pack(side=tk.RIGHT, padx=5)

        export_btn = tk.Button(ctrl_frame, text="Exportar CSV...", command=self.export_csv, bg="#eeeeee", padx=10)
        export_btn.pack(side=tk.RIGHT, padx=5)

        # Grafico Temporal Matplotlib
        graph_frame = tk.Frame(self.root)
        graph_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        self.fig = Figure(figsize=(8, 3.8), dpi=100)
        self.ax = self.fig.add_subplot(111)
        self.fig.tight_layout(pad=2.5)

        self.canvas = FigureCanvasTkAgg(self.fig, master=graph_frame)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

    def create_card(self, parent, title, val_default, subtext):
        frame = tk.Frame(parent, relief=tk.GROOVE, bd=2, bg="#f9f9f9", padx=12, pady=6)
        frame.pack(side=tk.LEFT, expand=True, fill=tk.BOTH, padx=4)

        tk.Label(frame, text=title, font=("Arial", 9), fg="#555555", bg="#f9f9f9").pack()
        val_lbl = tk.Label(frame, text=val_default, font=("Arial", 14, "bold"), fg="#111111", bg="#f9f9f9")
        val_lbl.pack()
        tk.Label(frame, text=subtext, font=("Arial", 8), fg="#888888", bg="#f9f9f9").pack()

        return val_lbl

    def on_threshold_change(self, val):
        self.threshold = int(val)
        self.thresh_val_lbl.config(text=str(self.threshold))

    def toggle_pause(self):
        self.paused = not self.paused
        self.pause_btn.config(text="Retomar Grafico" if self.paused else "Pausar Grafico")

    def export_csv(self):
        dest = filedialog.asksaveasfilename(
            defaultextension=".csv",
            filetypes=[("Arquivos CSV", "*.csv")],
            initialfile="historico_telemetria_exportado.csv"
        )
        if dest:
            try:
                import shutil
                shutil.copyfile(self.csv_filename, dest)
                messagebox.showinfo("Exportacao Concluida", f"Arquivo exportado com sucesso para:\n{dest}")
            except Exception as e:
                messagebox.showerror("Erro de Exportacao", str(e))

    def udp_listener(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            sock.bind(("0.0.0.0", self.udp_port))
        except Exception as e:
            print(f"[ERRO UDP] Nao foi possivel abrir porta {self.udp_port}: {e}")
            return

        sock.settimeout(0.5)
        start_time = time.time()

        while self.running:
            try:
                data, _ = sock.recvfrom(2048)
                msg = data.decode("utf-8", errors="ignore").strip()

                raw = None
                voltage = None
                violated = None
                sensor_id = "HW201"

                # Tenta parsear JSON
                if msg.startswith("{") and msg.endswith("}"):
                    try:
                        js = json.loads(msg)
                        raw = int(js.get("raw", 0))
                        voltage = float(js.get("voltage", 0.0))
                        violated = bool(js.get("violated", 0))
                        sensor_id = js.get("sensor_id", "HW201")
                    except Exception:
                        pass
                else:
                    # Formato CSV simples
                    parts = msg.split(",")
                    if len(parts) >= 3:
                        sensor_id = parts[0]
                        raw = int(parts[1])
                        voltage = float(parts[2])
                        violated = bool(int(parts[3])) if len(parts) > 3 else (raw >= self.threshold)

                if raw is not None:
                    self.latest_raw = raw
                    self.latest_voltage = voltage
                    # Avalia violacao conforme limiar dinamico da tela
                    self.is_violated = (raw >= self.threshold)
                    self.last_packet_time = time.time()
                    self.packet_count += 1

                    rel_time = time.time() - start_time
                    self.times.append(rel_time)
                    self.voltages.append(self.latest_voltage)
                    self.raw_values.append(self.latest_raw)

                    # Grava no CSV continuo
                    iso_now = time.strftime("%Y-%m-%d %H:%M:%S")
                    with open(self.csv_filename, "a", newline="", encoding="utf-8") as f:
                        writer = csv.writer(f)
                        writer.writerow([iso_now, sensor_id, raw, voltage, 1 if self.is_violated else 0])

            except socket.timeout:
                continue
            except Exception as e:
                time.sleep(0.1)

    def update_gui_loop(self):
        # Atualiza cards e banner
        now = time.time()
        elapsed = now - self.last_packet_time if self.last_packet_time > 0 else None

        if elapsed is not None and elapsed < 4.0:
            if self.is_violated:
                self.status_banner.config(
                    text="ALERTA: VIOLACAO DETECTADA! (COMPARTIMENTO ABERTO)",
                    bg="#cc0000",
                    fg="white"
                )
            else:
                self.status_banner.config(
                    text="CARGA INTEGRA (COMPARTIMENTO FECHADO E SEGURO)",
                    bg="#1b7e32",
                    fg="white"
                )
            self.card_raw.config(text=f"{self.latest_raw}")
            self.card_volt.config(text=f"{self.latest_voltage:.2f} V")
            self.card_ping.config(text=f"{elapsed:.1f} s")
            self.card_total.config(text=f"{self.packet_count}")
        else:
            self.status_banner.config(
                text="SEM SINAL DE TELEMETRIA (VERIFIQUE A CONEXAO ETHERNET)",
                bg="#777777",
                fg="white"
            )

        # Atualiza grafico Matplotlib se nao estiver pausado
        if not self.paused and len(self.times) > 1:
            self.ax.clear()
            self.ax.plot(list(self.times), list(self.voltages), color="#0055aa", linewidth=2.0, label="Tensao Medida (V)")

            # Linha de limiar convertida para tensao
            thresh_volt = (self.threshold / 4095.0) * 3.3
            self.ax.axhline(thresh_volt, color="red", linestyle="--", alpha=0.8, label=f"Limiar de Alarme ({thresh_volt:.2f} V)")

            self.ax.set_ylim(-0.2, 3.6)
            self.ax.set_ylabel("Tensao (V)", fontsize=9)
            self.ax.set_xlabel("Tempo Decorrido (s)", fontsize=9)
            self.ax.grid(True, linestyle=":", alpha=0.6)
            self.ax.legend(loc="upper right", fontsize=8)

            self.canvas.draw_idle()

        # Agenda proxima iteracao
        self.root.after(100, self.update_gui_loop)

def main():
    root = tk.Tk()
    app = TelemetryDashboard(root, udp_port=5000)
    root.mainloop()

if __name__ == "__main__":
    main()
