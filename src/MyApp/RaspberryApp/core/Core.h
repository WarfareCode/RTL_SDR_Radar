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

class Core : public QObject
{
    Q_OBJECT
    int sizeLog = 100;
    QTimer _timer;
    MainWindow _mainWindow;

    QSharedPointer<IPoolObject> _poolObjects = nullptr;
    QSharedPointer<IDataController> _dataController = nullptr;
    QSharedPointer<IReciverDevice> _device = nullptr;
    QSharedPointer<IDemodulator> _demodulator = nullptr;
    QSharedPointer<ILogger> _logger = nullptr;

public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    void init();
signals:

public slots:
    void slotTimeout();

};

#endif // CORE_H
