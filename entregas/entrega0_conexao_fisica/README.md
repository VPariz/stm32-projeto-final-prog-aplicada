# Entrega 0 — Conexao Fisica do Sensor com o Kit Desligado

**Autor:** Joao Victor Pariz  
**Sensor (Nº 3):** Modulo Sensor de Obstaculo Infravermelho Reflexivo Digital (HW-201)

## Mapeamento Pino a Pino (Alimentacao Estrita em 3,3 V)

| Pino no Sensor HW-201 | Conector na STM32MP157D-DK1 | Posicao Exata (Direita p/ Esquerda) |
| :--- | :--- | :--- |
| **VCC** | Bloco da Direita (8 furos) | **Pino 4** (`3V3`) |
| **GND** | Bloco da Direita (8 furos) | **Pino 7** (`GND`) |
| **OUT** | Bloco da Esquerda (6 furos - ADC) | **Pino 5** (Entrada ADC) |

## Fotos de Verificacao
* [Conexao nos headers da placa desligada](conexao_placa_desligada.jpg)
* [Conexao nos pinos do sensor HW-201](conexao_sensor_pinos.jpg)
