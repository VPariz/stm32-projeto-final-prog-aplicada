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
