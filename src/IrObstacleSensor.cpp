// Joao Victor Pariz
// Projeto Final - Sensor HW-201
// SE/3 - Eletronica - Programacao Aplicada

#include "IrObstacleSensor.hpp"

IrObstacleSensor::IrObstacleSensor(
    const std::string& path,
    int threshold,
    double vref,
    int maxAdc
) : devicePath(path),
    rawThreshold(threshold),
    lastRawValue(0),
    referenceVoltage(vref),
    maxAdcResolution(maxAdc) {
}

int IrObstacleSensor::readRaw() {
    std::ifstream file(devicePath);
    if (!file.is_open()) {
        throw std::runtime_error("Falha ao abrir canal IIO no caminho: " + devicePath);
    }

    int rawValue = 0;
    if (!(file >> rawValue)) {
        throw std::runtime_error("Falha ao ler dados numericos do canal IIO: " + devicePath);
    }

    lastRawValue = rawValue;
    return lastRawValue;
}

double IrObstacleSensor::getVoltage() const {
    if (maxAdcResolution <= 0) {
        return 0.0;
    }
    double voltage = (static_cast<double>(lastRawValue) / static_cast<double>(maxAdcResolution)) * referenceVoltage;
    if (voltage < 0.0) voltage = 0.0;
    if (voltage > referenceVoltage) voltage = referenceVoltage;
    return voltage;
}

double IrObstacleSensor::readVoltage() {
    readRaw();
    return getVoltage();
}

bool IrObstacleSensor::isObstacleDetected() const {
    // No sensor HW-201 com comparador LM393, quando ha reflexao (objeto proximo),
    // a saida OUT comuta para nivel logico BAIXO (0V / valor ADC proximo de 0).
    return lastRawValue < rawThreshold;
}

bool IrObstacleSensor::isCompartmentViolated() const {
    // No contexto do monitoramento de carga sensivel:
    // Sensor voltado para a face interna da tampa do compartimento:
    // - Tampa fechada (normal): feixe reflete na tampa -> obstaculo presente (lastRawValue < rawThreshold).
    // - Tampa aberta (violacao): feixe se propaga livremente -> sem reflexao -> nivel ALTO (lastRawValue >= rawThreshold).
    return !isObstacleDetected();
}

std::string IrObstacleSensor::getFormattedStatus() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "[SENSOR HW-201] Bruto: " << lastRawValue
        << " (" << getVoltage() << " V)"
        << " | Obstaculo: " << (isObstacleDetected() ? "PRESENTE" : "AUSENTE")
        << " | Carga: " << (isCompartmentViolated() ? "VIOLACAO DETECTADA!" : "INTEGRA (SEGURA)");
    return oss.str();
}

int IrObstacleSensor::getLastRawValue() const {
    return lastRawValue;
}

int IrObstacleSensor::getThreshold() const {
    return rawThreshold;
}

void IrObstacleSensor::setThreshold(int newThreshold) {
    rawThreshold = newThreshold;
}

const std::string& IrObstacleSensor::getDevicePath() const {
    return devicePath;
}

void IrObstacleSensor::setDevicePath(const std::string& newPath) {
    devicePath = newPath;
}
