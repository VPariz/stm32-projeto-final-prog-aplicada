# IoT Telemetry & Physical Tamper Detection with STM32MP157D and Linux IIO

[![C++17 CI](https://github.com/VPariz/stm32-projeto-final-prog-aplicada/actions/workflows/ci.yml/badge.svg)](https://github.com/VPariz/stm32-projeto-final-prog-aplicada/actions/workflows/ci.yml)
[![Hardware](https://img.shields.io/badge/Board-STM32MP157D--DK1-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white)](https://www.st.com/en/evaluation-tools/stm32mp157d-dk1.html)
[![Kernel](https://img.shields.io/badge/Kernel-Linux_IIO-E95420?style=flat-square&logo=linux&logoColor=white)](https://www.kernel.org/doc/html/latest/driver-api/iio/index.html)
[![Language](https://img.shields.io/badge/C%2B%2B-17_OOP-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Dashboard](https://img.shields.io/badge/UI-Python_Tkinter-3776AB?style=flat-square&logo=python&logoColor=white)](https://python.org)
[![License](https://img.shields.io/badge/License-MIT-green.svg?style=flat-square)](LICENSE)

Embedded IoT telemetry platform for sensitive payload integrity monitoring and real-time physical tamper detection, running in user space on Embedded Linux atop the **STM32MP157D-DK1** heterogeneous SoC (Dual ARM Cortex-A7 @ 800 MHz + ARM Cortex-M4 coprocessor @ 209 MHz).

The system interfaces with reflective infrared proximity sensors natively through the **Linux Industrial I/O (IIO)** kernel subsystem, processes voltage telemetry via an object-oriented C++17 driver class, transmits telemetry packets over low-latency **UDP sockets**, and streams live real-time traces to a Python visualization dashboard.

---

## 🏗️ System Architecture

```mermaid
flowchart TD
    subgraph SENSING["Physical Layer"]
        HW201["Reflective Optical IR Sensor<br/>(HW-201 / LM393 Module)"] -->|Analog 0.0V to 3.3V| ADC_PIN["ADC Header (Pin 5 / CN16)<br/>STM32MP157D-DK1"]
    end

    subgraph EMBEDDED_LINUX["Embedded Linux (OpenSTLinux)"]
        ADC_PIN --> KERNEL["Linux Kernel IIO Driver<br/>(stm32-adc)"]
        KERNEL --> SYSFS["Virtual Sysfs Filesystem<br/>/sys/bus/iio/devices/iio:device0/in_voltageX_raw"]
        SYSFS -->|std::ifstream| CPP_APP["C++17 Application<br/>(IrObstacleSensor Class)"]
        CPP_APP -->|Serialization| UDP_TX["UDP Transmitter<br/>(UdpTransmitter Class)"]
    end

    subgraph BASE_STATION["Base Station / Host PC"]
        UDP_TX -->|UDP Datagrams (Port 5005)| PY_GUI["Python GUI Dashboard<br/>(Real-Time Visualizer + CSV Logger)"]
    end
```

---

## 📁 Repository Structure

```
├── include/
│   ├── IrObstacleSensor.hpp        # Sensor class interface with dynamic IIO channel autodiscovery
│   └── UdpTransmitter.hpp          # Non-blocking POSIX UDP socket transmitter
├── src/
│   ├── IrObstacleSensor.cpp        # Sysfs virtual filesystem parser and millivolt conversion
│   ├── UdpTransmitter.cpp          # UDP socket implementation with error recovery
│   └── main.cpp                    # Main telemetry polling loop and event dispatch
├── scripts/
│   ├── cross_build.sh              # ARM cross-compilation pipeline script
│   ├── deploy_to_board.sh          # Automated SCP transfer and remote SSH execution
│   └── mock_sensor.py              # Synthetic sensor emulator for desktop bench validation
├── gui/
│   ├── gui_dashboard.py            # Real-time oscilloscope dashboard with tamper alerts
│   └── test_udp_receiver.py        # Terminal UDP packet verification utility
├── docs/
│   ├── Doxyfile                    # Doxygen API documentation configuration
│   └── hardware/                   # Wiring schematics, pinout tables, and assembly photos
├── Makefile                        # Native desktop build and ARM cross-compilation targets
└── LICENSE                         # MIT License
```

---

## ⚡ Key Technical Highlights

1. **Object-Oriented Linux IIO Driver (`IrObstacleSensor`):**
   * **Dynamic Kernel Channel Autodiscovery:** Scans `/sys/bus/iio/devices/iio:device0/` at runtime to detect active ADC channels.
   * **Hysteresis & Calibration:** Converts raw 12-bit ADC readings ($0$ to $4095$) to calibrated millivolts, applying software hysteresis to suppress false alarms from optical ambient noise.

2. **ARM Cross-Compilation Pipeline:**
   * **SDK:** `openstlinux-6.6-yocto-scarthgap-mpu` (`arm-openstlinux-linux-gnueabi-g++`).
   * **One-Step Deployment:** `deploy_to_board.sh` builds the ARM target, strips debug symbols, transfers the binary via SCP, and triggers remote daemon execution over SSH.

3. **Low-Latency UDP Telemetry:**
   * End-to-end transport latency under $2\text{ ms}$ on local subnets.
   * JSON telemetry payload format:
     ```json
     {"timestamp": 1729000000, "sensor_id": "HW201_CH1", "voltage": 2.75, "status": "TAMPER_ALERT"}
     ```

4. **Python Monitoring Dashboard:**
   * 60-second sliding history window with smooth real-time plot refresh.
   * Visual indicator banners for nominal monitoring vs. perimeter breach.
   * Automated CSV telemetry logging with ISO 8601 timestamps.

---

## 🚀 Building & Running

### 1. Local Desktop Simulation:
```bash
make clean && make
./bin/monitor_app --mock
```

### 2. ARM Cross-Compilation for STM32MP1:
```bash
# Source OpenSTLinux Yocto SDK environment
source /opt/st/stm32mp1/environment-setup-armv7ve-t2hf-neon-vfpv4-openstlinux-linux-gnueabi

# Build ARM binary
make CROSS_COMPILE=arm-openstlinux-linux-gnueabi-

# Deploy and execute on target board
./scripts/deploy_to_board.sh 192.168.1.100 root
```

### 3. Launch Desktop GUI Dashboard:
```bash
pip install -r requirements.txt
python gui/gui_dashboard.py
```

---

## 👤 Author
* **João Victor Tavares Pariz** ([@VPariz](https://github.com/VPariz)) — Electronics Engineering, Military Institute of Engineering (IME).

Developed at the **Department of Electrical & Electronics Engineering**, **Instituto Militar de Engenharia (IME)**, Rio de Janeiro, Brazil.

---

## 📜 License
This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
