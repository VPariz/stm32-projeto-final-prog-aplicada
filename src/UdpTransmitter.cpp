// Joao Victor Pariz
// Projeto Final - Sensor HW-201
// SE/3 - Eletronica - Programacao Aplicada

#include "UdpTransmitter.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <cstdint>

UdpTransmitter::UdpTransmitter(const std::string& ip, int port, const std::string& id)
    : serverIp(ip), serverPort(port), sensorId(id), sockfd(INVALID_SOCKET), initialized(false) {
    initSocket();
}

UdpTransmitter::~UdpTransmitter() {
    cleanupSocket();
}

void UdpTransmitter::initSocket() {
#ifdef _WIN32
    WSADATA wsaData;
    int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaRes != 0) {
        throw std::runtime_error("Falha ao inicializar WinSock.");
    }
#endif

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
#ifdef _WIN32
        WSACleanup();
#endif
        throw std::runtime_error("Falha ao criar socket UDP.");
    }

    std::memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(static_cast<uint16_t>(serverPort));

#ifdef _WIN32
    destAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());
#else
    if (inet_pton(AF_INET, serverIp.c_str(), &destAddr.sin_addr) <= 0) {
        cleanupSocket();
        throw std::runtime_error("Endereco IP invalido: " + serverIp);
    }
#endif

    initialized = true;
}

void UdpTransmitter::cleanupSocket() {
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
    }
#ifdef _WIN32
    WSACleanup();
#endif
    initialized = false;
}

bool UdpTransmitter::sendRawMessage(const std::string& message) {
    if (!initialized || sockfd == INVALID_SOCKET) {
        return false;
    }

    int sentBytes = sendto(
        sockfd,
        message.c_str(),
        static_cast<int>(message.length()),
        0,
        reinterpret_cast<struct sockaddr*>(&destAddr),
        sizeof(destAddr)
    );

    return (sentBytes != SOCKET_ERROR);
}

bool UdpTransmitter::sendTelemetryJson(
    int rawValue,
    double voltage,
    bool isViolated,
    long long timestampMs
) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "{"
        << "\"sensor_id\":\"" << sensorId << "\","
        << "\"raw\":" << rawValue << ","
        << "\"voltage\":" << voltage << ","
        << "\"unit\":\"V\","
        << "\"violated\":" << (isViolated ? 1 : 0) << ","
        << "\"timestamp\":" << timestampMs
        << "}\n";

    return sendRawMessage(oss.str());
}

bool UdpTransmitter::sendTelemetryCsv(int rawValue, double voltage, bool isViolated) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << sensorId << ","
        << rawValue << ","
        << voltage << ","
        << (isViolated ? 1 : 0) << "\n";

    return sendRawMessage(oss.str());
}

void UdpTransmitter::setServerIp(const std::string& newIp) {
    serverIp = newIp;
    cleanupSocket();
    initSocket();
}

void UdpTransmitter::setServerPort(int newPort) {
    serverPort = newPort;
    cleanupSocket();
    initSocket();
}
