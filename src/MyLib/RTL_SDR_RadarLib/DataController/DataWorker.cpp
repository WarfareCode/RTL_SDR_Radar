#include "DataWorker.h"

DataWorker::DataWorker(QSharedPointer<IReciverDevice> dev,
                       QSharedPointer<IDemodulator> dem,
                       size_t dataSize) :
                                          _device(dev),
                                          _demod(dem),
                                          _dataVectorSize(dataSize)
{
    qDebug()<<"create DataWorker";
    _dataVector.resize(int(_dataVectorSize));
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

    ///emit finished();
}

void DataWorker::getData()
{
    //qDebug()<<"DataWorker::getData() thread id ="<<QThread::currentThreadId();
    if(_mutex.tryLock(10))
    {
        _mutex.unlock();
        _condition.wakeAll();
    }
}


void DataWorker::exec()
{
    _abort = false;
    forever
    {
        //qDebug()<<"DBWorker::exec() thread id ="<<QThread::currentThreadId();
        _mutex.lock();

        if(!_device.isNull() && _device->isOpenDevice())
        {
            const uint8_t* ptrData = _device->getDataBlockPtr(size_t(MODES_DATA_LEN));

            if(ptrData != nullptr)
            {
                if(_dataVector.size() < _dataVectorSize)
                    _dataVector.resize(_dataVectorSize);

                /* Move the last part of the previous buffer, that was not processed,
                * on the start of the new buffer. */
                memcpy(_dataVector.data(), _dataVector.data() + MODES_DATA_LEN, MODES_FULL_LEN_OFFS);
                /* Read the new data. */
                memcpy(_dataVector.data() + MODES_FULL_LEN_OFFS, ptrData, size_t(MODES_DATA_LEN));
            }

            if(!_demod.isNull())
                _demod->demodulate(_dataVector,nullptr);

            if(!_dsp.isNull())
                _dsp->makeAll(_dataVector);
        }

        if(_abort)
            break;

        _condition.wait(&_mutex);
        _mutex.unlock();

    }
    _mutex.unlock();
    _condition.wakeAll();

    qDebug()<<"terminate thread id" << QThread::currentThreadId();
    emit finished();
}
