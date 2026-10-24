"""
# Joao Victor Pariz
# Projeto Final - Sensor HW-201
# SE/3 - Eletronica - Programacao Aplicada

Script de simulacao de sensor para testes em bancada caseira (PC).
Gera leituras alternadas simulando tampa fechada (carga integra) e tampa aberta (violacao).

Uso:
    python scripts/mock_sensor.py [caminho_arquivo] [intervalo_segundos]
"""

import sys
import time
import os

def main():
    target_file = sys.argv[1] if len(sys.argv) > 1 else "mock_sensor_raw.txt"
    interval = float(sys.argv[2]) if len(sys.argv) > 2 else 3.0

    target_dir = os.path.dirname(target_file)
    if target_dir and not os.path.exists(target_dir):
        os.makedirs(target_dir, exist_ok=True)

    print("=== SIMULADOR DE SENSOR DE OBSTACULO IR (HW-201) ===")
    print(f"Arquivo alvo: {os.path.abspath(target_file)}")
    print(f"Intervalo: {interval}s")
    print("Pressione Ctrl+C para encerrar.\n")

    states = [
        (120, "TAMPA FECHADA / REFLUXO IR PRESENTE -> CARGA INTEGRA (SEGURA)"),
        (4050, "TAMPA ABERTA / SEM REFLEXAO -> VIOLACAO DETECTADA!")
    ]

    idx = 0
    cycle = 0

    try:
        while True:
            cycle += 1
            raw_val, desc = states[idx]
            with open(target_file, "w", encoding="utf-8") as f:
                f.write(f"{raw_val}\n")
            print(f"[{cycle:03d}] Gravado: {raw_val:4d} ADC -> {desc}")
            idx = (idx + 1) % len(states)
            time.sleep(interval)
    except KeyboardInterrupt:
        print("\n[INFO] Simulador encerrado.")

if __name__ == "__main__":
    main()
