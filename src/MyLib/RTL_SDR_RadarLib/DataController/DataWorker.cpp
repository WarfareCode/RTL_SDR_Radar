#include "DataWorker.h"
#include <QThreadPool>

#include <sys/types.h>
#include <unistd.h>

#include "NetworkWorker.h"



using namespace std::chrono;

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
    _device.clear();
    _demod.clear();
    qDebug()<<"delete DataWorker";
}

void DataWorker::exec()
{
    _abort = false;
    forever
    {
        if(_abort)
            break;

        QMutexLocker lock(&_mutex);
        processData();
    }

    qDebug()<<"terminate thread id" << QThread::currentThreadId();
    emit finished();
}

bool DataWorker::processData()
{
    if(_device.isNull() || !_device->isOpenDevice())
        return false;

    const uint8_t* ptrData = _device->getDataBlockPtr(size_t(MODES_DATA_LEN));

    if(ptrData == nullptr)
        return false;

    if(_dataVector.size() < _dataVectorSize)
        _dataVector.resize(_dataVectorSize);

    /* Move the last part of the previous buffer, that was not processed,
            * on the start of the new buffer. */
    memcpy(_dataVector.data(),
           _dataVector.data() + MODES_DATA_LEN,
           MODES_FULL_LEN_OFFS);
    /* Read the new data. */
    memcpy(_dataVector.data() + MODES_FULL_LEN_OFFS,
           ptrData,
           size_t(MODES_DATA_LEN));

    if(_demod.isNull())
        return false;

    _demod->setDataForDemodulate(_dataVector);
    QThreadPool::globalInstance()->start(_demod.data());

    if(!_dsp.isNull())
    {
        _dsp->makeAll(_dataVector);
    }

    QThreadPool::globalInstance()->waitForDone();
    return true;
}
