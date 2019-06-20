#include "DataController.h"

//#include "DataWorker.h"
#include "DataWorkerNetSender.h"

DataController::DataController(QSharedPointer<IReciverDevice> dev,
                               QSharedPointer<IDemodulator> dem,
                               TYPE_WORKER typeWorker)
{
    qDebug()<<"create DataController";
    _dataThread = new QThread();
    switch (typeWorker)
    {
        case TYPE_WORKER::DATA_STREAM :
            _worker = std::unique_ptr<IWorker>(new DataWorker(dev,dem));
            break;

        case TYPE_WORKER::DATA_TO_NETWORK :
            _worker = std::unique_ptr<IWorker>(new DataWorkerNetSender(dev,dem));
            break;
    }

    if(_worker)
    {
        _worker->moveToThread(_dataThread);

        QObject::connect(_dataThread,
                         &QThread::started,
                         _worker.get(),
                         &IWorker::exec);

        QObject::connect(_worker.get(),
                         &IWorker::finished,
                         _dataThread,
                         &QThread::quit);
    }
}

DataController::~DataController()
{
    if(_dataThread->isRunning())
        _worker->abortExec();
    _dataThread->wait();
    _worker.reset();
    qDebug()<<"delete DataController";
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
