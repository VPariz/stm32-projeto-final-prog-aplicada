# Entrega 1 — Classe C++ de Leitura do Sensor Documentada com Doxygen

**Grupo:** Joao Victor Pariz, Mateus Roma, Daniel Coimbra  
**Sensor (Nº 3):** Modulo Sensor de Obstaculo Infravermelho Reflexivo Digital (HW-201)

## Arquivos desta Entrega
* [`include/IrObstacleSensor.hpp`](include/IrObstacleSensor.hpp): Declaracao da classe C++ encapsulada (zero variaveis globais) documentada com tags Doxygen (`@file`, `@class`, `@brief`, `@param`, `@return`, `@throws`).
* [`src/IrObstacleSensor.cpp`](src/IrObstacleSensor.cpp): Implementacao da leitura do canal IIO da placa (`/sys/bus/iio/devices/iio:device0/in_voltageX_raw`) com fallback automatico de deteccao de canal ativo (`autoDetectIioChannel()`).
* [`src/main.cpp`](src/main.cpp): Programa principal enxuto que instancia `IrObstacleSensor` e executa a leitura na placa.
* [`Doxyfile`](Doxyfile): Arquivo de configuracao do Doxygen (HTML disponivel em [`../../docs/doxygen/html/index.html`](../../docs/doxygen/html/index.html)).

## Como Compilar e Gerar o Doxygen
```bash
# Gerar documentacao Doxygen:
doxygen Doxyfile

# Compilar para o kit ARM (STM32MP157D-DK1):
arm-buildroot-linux-gnueabihf-g++ -Wall -Wextra -std=c++17 -Iinclude src/IrObstacleSensor.cpp src/main.cpp -o app_sensor_arm
```
