#include "DataController.h"

DataController::DataController(QSharedPointer<IReciverDevice> dev,
                               QSharedPointer<IDemodulator> dem)
{
    _dataThread = new QThread();

    if(dev)
        _worker = new DataWorker(dev,dem);
    else
        _worker = new DataWorkerImit();

    _worker->moveToThread(_dataThread);

    QObject::connect(_dataThread,
                     &QThread::started,
                     _worker,
                     &IWorker::exec);

    QObject::connect(_worker,
                     &IWorker::finished,
                     _dataThread,
                     &QThread::quit);

    QObject::connect(_worker,
                     &IWorker::finished,
                     _worker,
                     &IWorker::deleteLater);

    QObject::connect(_dataThread,
                     &QThread::finished,
                     _dataThread,
                     &QThread::deleteLater);
}

DataController::~DataController()
{
    if(_dataThread->isRunning())
        _worker->abortExec();
}

uint8_t *DataController::getDataToChart()
{
    qDebug()<<"DataController::getDataToChart() thread id ="<<QThread::currentThreadId();
    _worker->getData();
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
