# Entrega 3 — Envio da Leitura via UDP e Documentacao do Protocolo

**Autor:** Joao Victor Pariz

## Arquivos desta Entrega
* [`include/UdpTransmitter.hpp`](include/UdpTransmitter.hpp): Classe C++ encapsulada para envio de datagramas UDP.
* [`src/UdpTransmitter.cpp`](src/UdpTransmitter.cpp): Implementacao de sockets POSIX/WinSock e serializacao JSON/CSV.
* [`test_udp_receiver.py`](test_udp_receiver.py): Servidor UDP de teste para recepcao de pacotes na porta 5000.

## Especificacao do Protocolo UDP (Porta 5000)
Formato JSON transmitido a cada leitura:
```json
{"sensor_id":"G3_IR_HW201","raw":150,"voltage":0.12,"unit":"V","violated":0,"timestamp":1729600000000}
```

### Campos do Datagrama JSON

| Campo | Tipo | Descricao |
| :--- | :---: | :--- |
| `sensor_id` | `string` | Identificador do sensor embarcado (`"G3_IR_HW201"`). |
| `raw` | `int` | Leitura digital bruta do canal IIO (`in_voltageX_raw`, 0 a 4095). |
| `voltage` | `double` | Tensao eletrica convertida em Volts (`0.00` a `3.30`). |
| `unit` | `string` | Unidade fisica da grandeza medida (`"V"`). |
| `violated` | `int` | Flag de violacao do compartimento (`0` = integro, `1` = violado). |
| `timestamp` | `int64` | Instante da leitura em milissegundos (epoca UNIX). |

## Compilacao e Execucao com Envio UDP
```bash
# 1. No servidor/PC, iniciar o receptor UDP na porta 5000:
python3 gui/test_udp_receiver.py

# 2. Na placa STM32MP157D-DK1 (ou localmente no PC), executar enviando para o IP do servidor:
./app_sensor_arm /sys/bus/iio/devices/iio:device0/in_voltage0_raw 0 192.168.42.10 5000
```
