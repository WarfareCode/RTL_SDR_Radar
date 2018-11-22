#include "DataWorker.h"

DataWorker::DataWorker(QSharedPointer<IReciverDevice> dev,
                       QSharedPointer<IDemodulator> dem) :
    _device(dev),
    _demod(dem)
{
    qDebug()<<"create DataWorker";
    _dataVector.resize(MODES_DATA_LEN);
}

DataWorker::~DataWorker()
{
    qDebug()<<"delete DataWorker";
}

void DataWorker::setLogger(ILogger *)
{

}

void DataWorker::setReciverDevice(QSharedPointer<IReciverDevice> dev)
{
    _device = dev;
}

void DataWorker::setDemodulator(QSharedPointer<IDemodulator> dem)
{
    _demod = dem;
}

void DataWorker::abortExec()
{
    _abort = true;
    _mutex.unlock();
    _condition.wakeAll();
    emit finished();
}

void DataWorker::getData()
{
    qDebug()<<"DataWorker::getData() thread id ="<<QThread::currentThreadId();
    qDebug()<<"DataWorker::getData() tryLock = " <<_mutex.tryLock(1);
    _mutex.unlock();
    _condition.wakeAll();
}


void DataWorker::exec()
{
    _abort = false;
    forever
    {
        qDebug()<<"DBWorker::exec() thread id ="<<QThread::currentThreadId();
        _mutex.lock();

        if(!_device.isNull() && _device->isOpenDevice())
            _device->readDataBlock(_dataVector, _dataVector.size());

        qDebug()<<"DBWorker::exec() befor wait";
        _condition.wait(&_mutex);
        qDebug()<<"DBWorker::exec() after wait";
        _mutex.unlock();
        if(_abort)
            break;
    }

    emit finished();
}
