"""
# Joao Victor Pariz
# Projeto Final - Sensor HW-201
# SE/3 - Eletronica - Programacao Aplicada

Receptor UDP em linha de comando para testes de bancada e homologacao do protocolo.
Escuta na porta UDP especificada (padrao: 5000) e exibe os datagramas recebidos.

Uso:
    python scripts/test_udp_receiver.py [porta]
"""

import sys
import socket
import json
import time

def main():
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 5000
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", port))

    print("=========================================================")
    print("  Instituto Militar de Engenharia - IME (SE/3)")
    print(f"  Receptor UDP de Telemetria - Escutando na porta {port}")
    print("=========================================================\n")
    print("Aguardando datagramas UDP... (Pressione Ctrl+C para sair)\n")

    count = 0
    try:
        while True:
            data, addr = sock.recvfrom(2048)
            count += 1
            raw_text = data.decode("utf-8", errors="ignore").strip()
            hora = time.strftime("%H:%M:%S")

            try:
                js = json.loads(raw_text)
                print(f"[{count:04d} | {hora}] De {addr[0]}:{addr[1]}")
                print(f"       Sensor:  {js.get('sensor_id')}")
                print(f"       ADC Raw: {js.get('raw')} | Tensao: {js.get('voltage')} V")
                print(f"       Status:  {'VIOLACAO DETECTADA!' if js.get('violated') else 'CARGA INTEGRA'}")
                print(f"       Payload: {raw_text}\n")
            except Exception:
                print(f"[{count:04d} | {hora}] De {addr[0]}:{addr[1]} -> {raw_text}")

    except KeyboardInterrupt:
        print("\n[INFO] Receptor encerrado.")
    finally:
        sock.close()

if __name__ == "__main__":
    main()
