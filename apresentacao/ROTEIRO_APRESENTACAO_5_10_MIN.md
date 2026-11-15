# Roteiro de Apresentação (5 a 10 Minutos) — Projeto Final (SE/3 - IME)

**Grupo 3:** João Victor Tavares Pariz, Mateus Roma e Daniel Coimbra  
**Tempo total planejado:** ~7 a 8 minutos (dentro da faixa de 5 a 10 minutos).  
**Arquivos de apoio nesta pasta (`apresentacao/`):**
- [`apresentacao_interativa.html`](apresentacao_interativa.html) — Slides interativos Widescreen (navegue com `←` e `→`, links para os commits e player de vídeo no Slide 9).
- [`apresentacao_slides.pdf`](apresentacao_slides.pdf) — Versão em PDF (LaTeX Beamer 16:9).
- [`iniciar_demo_ao_vivo.bat`](iniciar_demo_ao_vivo.bat) — Script de 1 clique que compila o C++17, simula o canal IIO, abre o Dashboard Python e transmite pacotes UDP ao vivo na porta `5000`.

---

## Checklist dos Itens Obrigatórios da Apresentação
- [x] **Fluxograma** (Slide 3 HTML / Slide 3 PDF + `README.md`)
- [x] **Diagrama de Classes** (Slide 5 HTML / Slide 5 PDF + `README.md`)
- [x] **Instruções de compilação e uso** (Slide 6 HTML / Slide 6 PDF + `README.md`)
- [x] **Documentação do projeto, do código e do protocolo** (Slides 5 e 7 + Doxygen em `docs/doxygen/html/`)
- [x] **Descrição do sensor e funcionamento** (Slide 4 HTML / Slide 4 PDF)
- [x] **Capturas de tela da interface** (Slide 8 HTML / Slide 8 PDF + `docs/gui_carga_integra.png` e `docs/gui_alerta_violacao.png`)
- [x] **Análise dos valores medidos** (Slide 6 HTML / Slide 6 PDF + `README.md`)
- [ ] **Vídeo de demonstração na placa** (inserir `apresentacao/video_sensor.mp4` ou selecionar o vídeo gravado com o Coimbra no Slide 9!)

---

## Cronograma Minuto a Minuto (Fala Sugerida)

### 0:00 – 1:00 | Slides 1 e 2 — Introdução e Linha do Tempo por Commits
- **O que mostrar:** Abrir [`apresentacao_interativa.html`](apresentacao_interativa.html) em tela cheia (`F11`) e avançar para o Slide 2.
- **O que falar:**
  > *"Bom dia/Boa tarde. Nosso grupo implementou um sistema embarcado de monitoramento de integridade de carga sensível utilizando o kit STM32MP157D-DK1 e o sensor infravermelho reflexivo HW-201. Todo o desenvolvimento no GitHub foi organizado incrementalmente seguindo as 5 entregas da disciplina."*

---

### 1:00 – 2:00 | Slide 3 — Fluxograma Completo do Sistema
- **O que mostrar:** Slide 3 com o fluxo ponta a ponta (Sensor HW-201 → ADC STM32 → Linux IIO → Aplicação C++17 → Socket UDP → Dashboard Python → Alarme & CSV).
- **O que falar:**
  > *"Neste fluxograma vemos a arquitetura completa: o sensor HW-201 gera um sinal analógico proporcional à reflexão infravermelha; o driver stm32-adc do Linux expõe essa leitura no sysfs pelo subsistema Industrial I/O (IIO); nossa aplicação C++17 lê o valor a cada 250 ms, converte para Volts e transmite via socket UDP na porta 5000 para a central gráfica em Python, que atualiza o gráfico de 60 segundos, o alarme visual e o log em CSV."*

---

### 2:00 – 3:00 | Slide 4 — Entrega 0 (`commit ed3eb08`): Sensor HW-201 e Conexão Física
- **O que mostrar:** Slide 4 com a explicação *active-low* do LM393, tabela de pinos e as duas fotos reais da montagem.
- **O que falar:**
  > *"Na Entrega 0 (`ed3eb08`), fizemos a conexão física do módulo HW-201 com o kit totalmente desenergizado. Como o conversor A/D do STM32MP1 opera no limite de 3,3 V, ligamos o VCC estritamente no Pino 4 (`3V3`), o GND no Pino 7 e o sinal no Pino 5 (`ADC`). Quando a tampa está fechada, há reflexão IR e a saída cai para cerca de 0,68 V; ao abrir a tampa, perde-se a reflexão e a tensão sobe para ~2,90 V."*

---

### 3:00 – 4:15 | Slide 5 — Entregas 1 e 3 (`933267a`): Diagrama de Classes UML e Doxygen
- **O que mostrar:** Slide 5 exibindo as três classes (`IrObstacleSensor`, `UdpTransmitter` e `TelemetryDashboard`).
- **O que falar:**
  > *"Na modelagem Orientada a Objetos, não há nenhuma variável global ou função solta. `IrObstacleSensor` encapsula o caminho IIO, autodetecção de canal, conversão para Volts e diagnóstico. `UdpTransmitter` gerencia o socket UDP via RAII com compatibilidade dual Linux POSIX e Windows WinSock, serializando os dados em JSON ou CSV. No PC, `TelemetryDashboard` recebe os pacotes em thread dedicada. Toda a documentação C++ foi gerada automaticamente com Doxygen."*

---

### 4:15 – 5:30 | Slide 6 — Entrega 2 (`commit d4b8184`): Compilação, Uso e Análise dos Valores Medidos
- **O que mostrar:** Slide 6 com os comandos `make cross`, `scp`, execução na placa e a tabela comparativa de valores medidos.
- **O que falar:**
  > *"Na Entrega 2 (`d4b8184`), configuramos o `Makefile`, o script `cross_build.sh` e a pipeline de CI no GitHub Actions para compilar tanto nativamente quanto com a toolchain ARM (`arm-buildroot-linux-gnueabihf-g++`). Nas medições, adotamos o limiar padrão de 2000 na escala de 12 bits (~1,61 V): com tampa fechada registramos leituras típicas entre 100 e 900 (0,08 V a 0,75 V), e na abertura os valores saltam para 3500 a 4095 (2,80 V a 3,30 V)."*

---

### 5:30 – 6:30 | Slide 7 — Entrega 3 (`8fb8f14` → `779a547`): Protocolo UDP
- **O que mostrar:** Slide 7 com a sequência de commits da Entrega 3 e o datagrama JSON.
- **O que falar:**
  > *"A Entrega 3 foi dividida em commits modulares (`8fb8f14`, `e11bb71`, `7dad673` e `779a547`). A cada leitura, é enviado um datagrama UDP JSON na porta 5000 com `sensor_id`, `raw`, `voltage`, `unit`, a flag `violated` e o `timestamp` em milissegundos."*

---

### 6:30 – 8:00 | Slides 8, 9 e 10 — Capturas de Tela, Vídeo da Placa e Demo ao Vivo
- **O que mostrar:**
  1. **Slide 8:** Mostrar as capturas reais da GUI nos dois estados (`CARGA ÍNTEGRA` verde e `ALERTA: VIOLACAO DETECTADA!` vermelho).
  2. **Slide 9:** Reproduzir o vídeo gravado na placa STM32MP157D-DK1 com o sensor HW-201.
  3. **Slide 10:** Rodar `apresentacao\iniciar_demo_ao_vivo.bat` e mostrar ao vivo o gráfico subindo/descendo, o ajuste do slider de limiar e a exportação CSV.
