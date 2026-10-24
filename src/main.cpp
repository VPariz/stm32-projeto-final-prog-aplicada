// Joao Victor Pariz
// Projeto Final - Sensor HW-201
// SE/3 - Eletronica - Programacao Aplicada

#include <iostream>
#include <chrono>
#include <thread>
#include "IrObstacleSensor.hpp"
#include "UdpTransmitter.hpp"

int main(int argc, char* argv[]) {
    std::cout << "=========================================================\n";
    std::cout << "  Instituto Militar de Engenharia - IME (SE/3)\n";
    std::cout << "  Programacao Aplicada a Engenharia Eletrica\n";
    std::cout << "  Projeto: Monitoramento Inteligente de Carga Sensivel (IoT)\n";
    std::cout << "  Sensor: Obstaculo Infravermelho Reflexivo (HW-201)\n";
    std::cout << "=========================================================\n";

    std::string path = "/sys/bus/iio/devices/iio:device0/in_voltage0_raw";
    std::string serverIp = "127.0.0.1";
    int serverPort = 5000;
    int samples = 0; // 0 = modo continuo para telemetria em tempo real

    if (argc >= 2) {
        path = argv[1];
        std::cout << "[CONFIG] Caminho do sensor: " << path << "\n";
    } else {
        std::cout << "[CONFIG] Usando caminho padrao da placa: " << path << "\n";
    }

    if (argc >= 3) {
        serverIp = argv[2];
        std::cout << "[CONFIG] Servidor UDP IP: " << serverIp << "\n";
    }

    if (argc >= 4) {
        serverPort = std::stoi(argv[3]);
        std::cout << "[CONFIG] Servidor UDP Porta: " << serverPort << "\n";
    }

    if (argc >= 5) {
        samples = std::stoi(argv[4]);
    }

    std::cout << "[INIT] Inicializando sensor e transmissor UDP...\n";
    IrObstacleSensor sensor(path, 2000);
    UdpTransmitter transmitter(serverIp, serverPort, "G3_IR_HW201");

    std::cout << "[INFO] Telemetria iniciada ("
              << (samples == 0 ? "Modo Continuo (Pressione Ctrl+C para parar)" : std::to_string(samples) + " amostras")
              << ")...\n\n";

    int count = 0;
    while (samples == 0 || count < samples) {
        try {
            int raw = sensor.readRaw();
            double volt = sensor.getVoltage();
            bool violated = sensor.isCompartmentViolated();

            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            // Envia telemetria via socket UDP para a central
            transmitter.sendTelemetryJson(raw, volt, violated, ms);

            std::cout << "[" << std::setw(4) << std::setfill('0') << ++count << "] "
                      << sensor.getFormattedStatus()
                      << " -> [UDP ENVIADO]\n";
        } catch (const std::exception& e) {
            std::cerr << "[ERRO] " << e.what() << "\n";
            std::cerr << "Dica: Em desenvolvimento local no PC, passe o arquivo mock como argumento:\n";
            std::cerr << "      Exemplo: ./app_sensor mock_sensor_raw.txt 127.0.0.1 5000 10\n";
            return 1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250)); // 4 Hz de taxa de atualizacao
    }

    std::cout << "\n[INFO] Execucao concluida.\n";
    return 0;
}
