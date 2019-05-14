#ifndef CORE_H
#define CORE_H

#include <QObject>

#include <QDebug>
#include <chrono>
#include <QTimer>
#include <QFile>

#include "../MyLib/RTL_SDR_RadarLib/PoolObject/PoolObject.h"
#include "../MyLib/RTL_SDR_RadarLib/Logger/Logger.h"
#include "../MyLib/RTL_SDR_RadarLib/Carrier/Carrier.h"
#include "../MyLib/RTL_SDR_RadarLib/Carrier/ServiceLocator.h"
//#include "../MyLib/RTL_SDR_RadarLib/MapLib/MapLib.h"
#include "../MyLib/RTL_SDR_RadarLib/DataController/DataController.h"
#include "../MyLib/RTL_SDR_RadarLib/RTL_SDR_Reciver/RTL_SDR_Reciver.h"
#include "../MyLib/RTL_SDR_RadarLib/Demodulator/Demodulator.h"
#include "MainWindow.h"


class Core : public QObject
{
    Q_OBJECT
    QTimer _timer;
    MainWindow _mainWindow;

    QSharedPointer<IPoolObject> _poolObjects = nullptr;
    IDataController* _dataController = nullptr;
    QSharedPointer<IReciverDevice> _device = nullptr;
    QSharedPointer<IDemodulator> _demodulator = nullptr;

public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    void init();
signals:

public slots:
    void slotTimeout();

};

#endif // CORE_H
