#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTimer>

#include "ui/Mainwindow.h"

class IDataController;
class IPoolObject;
class IReciverDevice;
class IDemodulator;
class ILogger;
class INetworkWorker;

class Core : public QObject
{
    Q_OBJECT
    int sizeLog = 1000;
    QTimer _timer;
    MainWindow _mainWindow;

    QSharedPointer<IPoolObject> _poolObjects = nullptr;
    QSharedPointer<IDataController> _dataController = nullptr;
    QSharedPointer<IReciverDevice> _device = nullptr;
    QSharedPointer<IDemodulator> _demodulator = nullptr;
    QSharedPointer<ILogger> _logger = nullptr;
    QSharedPointer<INetworkWorker> _network = nullptr;

    const QString DEFAULT_IP = QString("127.0.0.1");
    uint16_t DEFAULT_PORT = 62000;

public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    void init();
signals:

public slots:
    void slotTimeout();

};

#endif // CORE_H
