// Joao Victor Pariz
// Projeto Final - Sensor HW-201
// SE/3 - Eletronica - Programacao Aplicada

#ifndef UDP_TRANSMITTER_HPP
#define UDP_TRANSMITTER_HPP

#include <string>
#include <stdexcept>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SOCKET;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define closesocket(s) close(s)
#endif

/**
 * @file UdpTransmitter.hpp
 * @brief Classe de transmissao de dados via datagramas UDP para o servidor central.
 * @details Modulo compativel com Linux POSIX (STM32MP1) e Windows (MinGW).
 *          Encapsula socket, enderecamento e serializacao das mensagens de telemetria.
 */

/**
 * @class UdpTransmitter
 * @brief Gerencia a conexao sem conexao (UDP) e envio de telemetria da carga monitorada.
 */
class UdpTransmitter {
private:
    std::string serverIp;        /**< Endereco IP do servidor central de destino */
    int serverPort;              /**< Porta UDP do servidor de monitoramento */
    std::string sensorId;        /**< Identificador unico do sensor/grupo */
    SOCKET sockfd;               /**< Descritor do socket UDP */
    struct sockaddr_in destAddr; /**< Estrutura de enderecamento de destino */
    bool initialized;            /**< Flag de estado de inicializacao do socket */

    void initSocket();
    void cleanupSocket();

public:
    /**
     * @brief Construtor da classe UdpTransmitter.
     * @param ip Endereco IP do servidor (padrao: 127.0.0.1 em casa ou 192.168.42.10 na bancada).
     * @param port Porta UDP de escuta do servidor (padrao: 5000).
     * @param id Identificador formal do sensor (padrao: "G3_IR_HW201").
     */
    explicit UdpTransmitter(
        const std::string& ip = "127.0.0.1",
        int port = 5000,
        const std::string& id = "G3_IR_HW201"
    );

    /**
     * @brief Destrutor da classe, encarregado de fechar o socket de forma segura.
     */
    virtual ~UdpTransmitter();

    /**
     * @brief Envia uma mensagem de texto arbitraria via UDP.
     * @param message Conteudo do datagrama a ser enviado.
     * @return true se o envio foi bem-sucedido, false caso contrario.
     */
    bool sendRawMessage(const std::string& message);

    /**
     * @brief Formata e transmite telemetria estruturada em JSON leve.
     * @param rawValue Leitura bruta do conversor A/D (0 a 4095).
     * @param voltage Tensao eletrica medida em Volts.
     * @param isViolated Booleano indicando violacao da carga (true = alarme).
     * @param timestampMs Timestamp em milissegundos da leitura.
     * @return true se enviado com sucesso; false caso contrario.
     */
    bool sendTelemetryJson(
        int rawValue,
        double voltage,
        bool isViolated,
        long long timestampMs
    );

    /**
     * @brief Formata e transmite telemetria estruturada em CSV simples.
     * @param rawValue Leitura bruta do ADC.
     * @param voltage Tensao calculada em Volts.
     * @param isViolated Estado de violacao da carga.
     * @return true se enviado com sucesso.
     */
    bool sendTelemetryCsv(int rawValue, double voltage, bool isViolated);

    /**
     * @brief Atualiza o IP do servidor em tempo de execucao.
     * @param newIp Novo endereco IP.
     */
    void setServerIp(const std::string& newIp);

    /**
     * @brief Atualiza a porta do servidor em tempo de execucao.
     * @param newPort Nova porta UDP.
     */
    void setServerPort(int newPort);
};

#endif // UDP_TRANSMITTER_HPP
