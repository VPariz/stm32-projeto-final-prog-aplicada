// Joao Victor Pariz
// Projeto Final - Sensor HW-201
// SE/3 - Eletronica - Programacao Aplicada

#ifndef IR_OBSTACLE_SENSOR_HPP
#define IR_OBSTACLE_SENSOR_HPP

#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

/**
 * @file IrObstacleSensor.hpp
 * @brief Classe para leitura e diagnostico do Sensor de Obstaculo IR HW-201 via IIO.
 * @details Este modulo realiza a leitura do conversor A/D da placa STM32MP157D-DK1
 *          atraves do subsistema Industrial I/O (IIO) do Linux (/sys/bus/iio/...).
 *          Respeita integralmente os principios de POO (zero variaveis globais).
 */

/**
 * @class IrObstacleSensor
 * @brief Encapsula a interface de leitura e interpretacao de estado do sensor HW-201.
 */
class IrObstacleSensor {
private:
    std::string devicePath;      /**< Caminho do arquivo IIO no sysfs ou arquivo mock */
    int rawThreshold;            /**< Limiar de decisao entre nivel logico baixo e alto */
    int lastRawValue;            /**< Ultimo valor numerico bruto lido do conversor ADC */
    double referenceVoltage;     /**< Tensao de referencia eletrica (3.3 V) */
    int maxAdcResolution;        /**< Resolucao maxima da escala do ADC (ex: 4095 para 12 bits) */

public:
    /**
     * @brief Construtor da classe com parametros padrao configuraveis.
     * @param path Caminho para o arquivo sysfs IIO ou arquivo simulador para testes.
     * @param threshold Limiar de decisao de alarme (padrao: 2000).
     * @param vref Tensao de referencia em Volts (padrao: 3.3 V).
     * @param maxAdc Escala maxima do conversor (padrao: 4095 para 12 bits).
     */
    explicit IrObstacleSensor(
        const std::string& path = "/sys/bus/iio/devices/iio:device0/in_voltage0_raw",
        int threshold = 2000,
        double vref = 3.3,
        int maxAdc = 4095
    );

    /**
     * @brief Destrutor padrao virtual da classe.
     */
    virtual ~IrObstacleSensor() = default;

    /**
     * @brief Le o valor bruto inteiro diretamente do arquivo do subsistema IIO.
     * @return Valor inteiro lido do conversor ADC (ex: 0 a 4095).
     * @throws std::runtime_error Se o arquivo nao puder ser aberto ou lido.
     */
    int readRaw();

    /**
     * @brief Converte o ultimo valor bruto para tensao equivalente em Volts.
     * @return Tensao calculada em Volts (0.0 V a 3.3 V).
     */
    double getVoltage() const;

    /**
     * @brief Atualiza a leitura e retorna a tensao calculada.
     * @return Tensao atualizada em Volts.
     */
    double readVoltage();

    /**
     * @brief Verifica se um obstaculo/reflexao infravermelha esta presente.
     * @details No sensor HW-201 (LM393), quando ha reflexao a saida vai para nivel BAIXO (0V).
     * @return true se reflexao detectada (leitura abaixo do limiar); false caso contrario.
     */
    bool isObstacleDetected() const;

    /**
     * @brief Avalia o estado de violacao da carga monitorada.
     * @details Compartimento fechado: reflexao continua (carga integra).
     *          Compartimento aberto: feixe desimpedido, saida em nivel ALTO (violacao!).
     * @return true se a carga foi aberta/violada; false se estiver integra e segura.
     */
    bool isCompartmentViolated() const;

    /**
     * @brief Retorna uma string formatada contendo o diagnostico completo atual.
     * @return String contendo valor bruto, tensao e estado de seguranca da carga.
     */
    std::string getFormattedStatus() const;

    /**
     * @brief Obtem o ultimo valor bruto lido sem novo acesso ao arquivo.
     * @return Ultimo valor ADC registrado.
     */
    int getLastRawValue() const;

    /**
     * @brief Obtem o limiar de decisao atual.
     * @return Valor inteiro do limiar.
     */
    int getThreshold() const;

    /**
     * @brief Permite ajuste dinamico do limiar de deteccao de alerta.
     * @param newThreshold Novo limiar numerico (0 a 4095).
     */
    void setThreshold(int newThreshold);

    /**
     * @brief Obtem o caminho configurado para o arquivo de leitura.
     * @return String com o caminho do arquivo.
     */
    const std::string& getDevicePath() const;

    /**
     * @brief Altera o caminho do arquivo de leitura em tempo de execucao.
     * @param newPath Novo caminho para o arquivo.
     */
    void setDevicePath(const std::string& newPath);
};

#endif // IR_OBSTACLE_SENSOR_HPP
