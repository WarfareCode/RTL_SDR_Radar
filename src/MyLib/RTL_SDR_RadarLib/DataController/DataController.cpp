#include "DataController.h"

DataController::DataController(QSharedPointer<IReciverDevice> dev,
                               QSharedPointer<IDemodulator> dem)
{
    qDebug()<<"create DataController";
    _dataThread = new QThread();

    if(dev)
        _worker = std::unique_ptr<IWorker>(new DataWorker(dev,dem));
    else
        _worker = std::unique_ptr<IWorker>(new DataWorkerImit());

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
