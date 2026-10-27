# Monitoramento IoT com STM32MP157D e Linux IIO

[![Hardware](https://img.shields.io/badge/Placa-STM32MP157D--DK1-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white)](https://www.st.com/en/evaluation-tools/stm32mp157d-dk1.html)
[![Kernel](https://img.shields.io/badge/Kernel-Linux_IIO-E95420?style=flat-square&logo=linux&logoColor=white)](https://www.kernel.org/doc/html/latest/driver-api/iio/index.html)
[![Linguagem](https://img.shields.io/badge/C%2B%2B-17_POO-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Dashboard](https://img.shields.io/badge/Interface-Python_Tkinter-3776AB?style=flat-square&logo=python&logoColor=white)](https://python.org)
[![Licença](https://img.shields.io/badge/Licen%C3%A7a-MIT-green.svg?style=flat-square)](LICENSE)

Sistema embarcado para monitoramento de integridade e detecção de violação física em cargas sensíveis, executando diretamente em espaço de usuário sobre Linux Embarcado na placa **STM32MP157D-DK1** (processador dual Cortex-A7 a 800 MHz + coprocessador Cortex-M4 a 209 MHz).

O projeto realiza a leitura de sensores infravermelhos reflexivos através do subsistema do Kernel **Linux Industrial I/O (IIO)**, processa os dados com uma classe em C++17 orientada a objetos, transmite o estado via **sockets UDP** de baixa latência e exibe os gráficos em tempo real em um **dashboard em Python**.

---

## Arquitetura do Sistema

```mermaid
flowchart TD
    subgraph SENSORIAMENTO["Camada Física"]
        HW201["Sensor Óptico Reflexivo IR<br/>(Módulo HW-201 / LM393)"] -->|Tensão 0.0V a 3.3V| ADC_PIN["Header ADC (Pino 5 / CN16)<br/>STM32MP157D-DK1"]
    end

    subgraph LINUX_EMBARCADO["Linux Embarcado (OpenSTLinux)"]
        ADC_PIN --> KERNEL["Driver IIO do Kernel Linux<br/>(stm32-adc)"]
        KERNEL --> SYSFS["Virtual Filesystem Sysfs<br/>/sys/bus/iio/devices/iio:device0/in_voltageX_raw"]
        SYSFS -->|std::ifstream| CPP_APP["Aplicação C++17<br/>(Classe IrObstacleSensor)"]
        CPP_APP -->|Serialização| UDP_TX["Transmissor UDP<br/>(Classe UdpTransmitter)"]
    end

    subgraph ESTACAO_BASE["Computador / Estação de Monitoramento"]
        UDP_TX -->|Datagramas UDP (Porta 5005)| PY_GUI["Interface Gráfica em Python<br/>(Visualização em Tempo Real + Log CSV)"]
    end
```

---

## Módulos do Projeto

```
├── include/
│   ├── IrObstacleSensor.hpp        # Interface da classe do sensor com auto-detecção IIO
│   └── UdpTransmitter.hpp          # Transmissor de sockets UDP de baixa latência
├── src/
│   ├── IrObstacleSensor.cpp        # Leitura dos arquivos virtuais sysfs e cálculo de tensão
│   ├── UdpTransmitter.cpp          # Implementação de sockets BSD UDP não bloqueantes
│   └── main.cpp                    # Loop principal de monitoramento e envio de telemetria
├── scripts/
│   ├── cross_build.sh              # Script de compilação cruzada para arquitetura ARM
│   ├── deploy_to_board.sh          # Envio automático via SCP e execução remota por SSH
│   └── mock_sensor.py              # Emulador de sensor para testes em bancada sem a placa
├── gui/
│   ├── gui_dashboard.py            # Dashboard gráfico com osciloscópio e alertas em tempo real
│   └── test_udp_receiver.py        # Receptor de teste no terminal para validação de pacotes
├── docs/
│   ├── Doxyfile                    # Configuração para geração de documentação Doxygen
│   └── hardware/                   # Diagramas de ligação, pinagem e fotos da montagem
├── Makefile                        # Compilação nativa no PC e compilação cruzada para ARM
└── LICENSE                         # Licença MIT
```

---

## Principais Destaques Técnicos

1. **Driver IIO Orientado a Objetos (`IrObstacleSensor`):**
   * **Auto-detecção de Canal do Kernel:** Faz a varredura dinâmica em `/sys/bus/iio/devices/iio:device0/` para encontrar o canal ADC correto em tempo de execução.
   * **Histerese e Calibração:** Converte a leitura de 12 bits do ADC ($0$ a $4095$) para milivolts, aplicando histerese configurável para evitar leituras falsas causadas por ruído óptico.

2. **Compilação Cruzada para ARM:**
   * **SDK Utilizado:** `openstlinux-6.6-yocto-scarthgap-mpu` (`arm-openstlinux-linux-gnueabi-g++`).
   * **Deploy com um Comando:** O script `deploy_to_board.sh` compila o binário, reduz os símbolos de depuração (*strip*), envia via rede para a placa e inicia o processo remotamente.

3. **Telemetria UDP em Tempo Real:**
   * Latência de envio inferior a $2\text{ ms}$ em rede local.
   * Formato padronizado de pacote:
     ```json
     {"timestamp": 1729000000, "sensor_id": "HW201_CH1", "tensao": 2.75, "status": "ALERTA_VIOLACAO"}
     ```

4. **Interface Gráfica de Monitoramento (Python):**
   * Gráfico de histórico deslizante de 60 segundos com taxa de atualização suave.
   * Indicadores visuais imediatos de estado normal e violação de barreira.
   * Registro em arquivo CSV com data e hora no padrão ISO 8601.

---

## Como Compilar e Executar

### 1. Testar localmente no computador (Modo Simulação):
```bash
make clean && make
./bin/monitor_app --mock
```

### 2. Compilar para a placa STM32MP1 (Compilação Cruzada):
```bash
# Carrega as variáveis do SDK da ST
source /opt/st/stm32mp1/environment-setup-armv7ve-t2hf-neon-vfpv4-openstlinux-linux-gnueabi

# Compila o binário ARM
make CROSS_COMPILE=arm-openstlinux-linux-gnueabi-

# Envia e roda na placa (informe o IP da placa e usuário)
./scripts/deploy_to_board.sh 192.168.1.100 root
```

### 3. Abrir o Dashboard Gráfico no PC:
```bash
pip install -r requirements.txt
python gui/gui_dashboard.py
```

---

## Autores
* **João Victor Tavares Pariz** ([@VPariz](https://github.com/VPariz)) — Engenharia Eletrônica, IME
* **Mateus Roma** — Engenharia Eletrônica, IME
* **Daniel Coimbra** — Engenharia Eletrônica, IME

Desenvolvido no **Departamento de Engenharia Elétrica e Eletrônica**, **Instituto Militar de Engenharia (IME)**, Rio de Janeiro.

---

## Licença
Este projeto está sob licença **MIT** — consulte o arquivo [LICENSE](LICENSE) para detalhes.
