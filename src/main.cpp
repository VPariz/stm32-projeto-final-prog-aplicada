// Joao Victor Pariz
// Projeto Final - Sensor HW-201
// SE/3 - Eletronica - Programacao Aplicada

#include <iostream>
#include "IrObstacleSensor.hpp"
#include "UdpTransmitter.hpp"

int main(int argc, char* argv[]) {
    std::string path = (argc >= 2) ? argv[1] : "/sys/bus/iio/devices/iio:device0/in_voltage0_raw";
    int samples = (argc >= 3) ? std::stoi(argv[2]) : 0;
    std::string serverIp = (argc >= 4) ? argv[3] : "192.168.42.10";
    int serverPort = (argc >= 5) ? std::stoi(argv[4]) : 5000;

    try {
        IrObstacleSensor sensor(path, 2000);
        UdpTransmitter transmitter(serverIp, serverPort, "G3_IR_HW201");
        sensor.runTelemetryLoop(transmitter, samples, 250);
    } catch (const std::exception& e) {
        std::cerr << "[ERRO] " << e.what() << "\n";
        return 1;
    }
    return 0;
}
