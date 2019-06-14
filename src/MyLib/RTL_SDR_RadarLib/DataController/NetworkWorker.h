#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <QTcpSocket>
#include <memory>
#include "INetworkWorker.h"
#include "ILogger.h"

class NetworkWorker : public INetworkWorker
{
    Q_OBJECT

    ILogger* _log = nullptr;
    IPackageController* _pkgCtrl = nullptr;
    std::unique_ptr<QTcpSocket> _socket = nullptr;

    void addDebugMsg(const QString& str);
public:
    NetworkWorker(const QString& ip,
                  uint16_t port);

    ~NetworkWorker() override;

    void setLogger(ILogger* log) override { _log = log; }
    void setController(IPackageController* ctrl) override { _pkgCtrl = ctrl; }


    /*!
     *   \brief Подключение к физическому устройству.
     *   \return результат операции
     */
    bool connect(const QString& ip,
                 uint16_t port,
                 uint16_t timeout = 1000) override;

    /*!
     * \brief Проверка состояние подключения
     * \return есть подключение или нет
     */
    bool isConnected() override;

    /*!
     * \brief отключение
     */
    void disconnect() override;
    /*!
     *  \brief Запись массива данных,
     *  используя ранее установленное подключение
     *  \param [in] byteArray - массив данных
     *  \return количество отправленных байт, -1 в случае ошибки
    */
    int64_t writeDatagramm(const QByteArray &byteArray) override;
    /*!
     *  \brief Запись массива данных,
     *  без использования установленного ранее подключения.
     *  Либо использование udp рассылки
     *  \param [in] ip - IP адрес получателя
     *  \param [in] port - порт получателя
     *  \param [in] byteArray - массив данных
     *  \return количество отправленных байт, -1 в случае ошибки
    */
    int64_t writeDatagramm(QString &ip,
                           uint16_t port,
                           const QByteArray &byteArray) override;
    /*!
     *  \brief Чтение массива данных
     *  \return количество прочтённых байт, -1 в случае ошибки
    */
    int64_t readDatagramm() override { return 0; }
};

#endif // NETWORKWORKER_H
