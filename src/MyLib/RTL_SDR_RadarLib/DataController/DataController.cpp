#include "DataController.h"

DataController::DataController(QSharedPointer<IReciverDevice> dev,
                               QSharedPointer<IDemodulator> dem)
{
    qDebug()<<"create DataController";
    _dataThread = new QThread();

    _worker = std::unique_ptr<IWorker>(new DataWorker(dev,dem));

    _worker->moveToThread(_dataThread);

    QObject::connect(_dataThread,
                     &QThread::started,
                     _worker.get(),
                     &IWorker::exec);

    QObject::connect(_worker.get(),
                     &IWorker::finished,
                     _dataThread,
                     &QThread::quit);
    //    QObject::connect(_worker.get(),
    //                     &IWorker::finished,
    //                     _worker.get(),
    //                     &IWorker::deleteLater);

    //    QObject::connect(_dataThread,
    //                     &QThread::finished,
    //                     _dataThread,
    //                     &QThread::deleteLater);
}

DataController::~DataController()
{
    if(_dataThread->isRunning())
        _worker->abortExec();
    _dataThread->wait();
    _worker.reset();

    qDebug()<<"delete DataController";
}

uint8_t *DataController::getDataToChart()
{
    //qDebug()<<"DataController::getDataToChart() thread id ="<<QThread::currentThreadId();
    //_worker->getData();
    return nullptr;
}

void DataController::run()
{
    if(_dataThread && _worker)
        _dataThread->start();
}

void DataController::stop()
{
    if(_dataThread && _worker)
    {
        _worker->abortExec();
        _dataThread->quit();
        _dataThread->wait();
    }
}

bool DataController::isRunning()
{
    return _dataThread->isRunning();
}

void DataController::setLogger(ILogger *log)
{
    if(_worker != nullptr)
        _worker->setLogger(log);
}

void DataController::setReciverDevice(QSharedPointer<IReciverDevice> dev)
{
    if(_worker != nullptr)
        _worker->setReciverDevice(dev);
}

void DataController::setDemodulator(QSharedPointer<IDemodulator> dem)
{
    if(_worker != nullptr)
        _worker->setDemodulator(dem);
}

void DataController::setDSP(QSharedPointer<IDSP> dsp)
{
    if(_worker != nullptr)
        _worker->setDSP(dsp);
}

void DataController::setNetworkModule(QSharedPointer<INetworkWorker> net)
{
    if(_worker != nullptr)
        _worker->setNetworkModule(net);
}
