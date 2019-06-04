#ifndef DATAWORKER_H
#define DATAWORKER_H

#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QWaitCondition>
#include <chrono>
#include <atomic>

#include "../../../include/IWorker.h"
#include "../../../include/dsp/IDSP.h"
#include "../../../../import/sdr_dev/include/constant.h"

class DataWorker : public IWorker
{
    Q_OBJECT

    QSharedPointer<IReciverDevice> _device;
    QSharedPointer<IDemodulator> _demod;
    QSharedPointer<IDSP> _dsp;
    QVector<uint8_t> _dataVector;
    QMutex _mutex;
    QWaitCondition _condition;
    std::atomic<bool> _abort;
    size_t _dataVectorSize = MODES_DATA_LEN + MODES_FULL_LEN_OFFS;
    bool processData();
    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _lastRequest;

    std::atomic<uint32_t> _ms_sleep;

public:
    DataWorker(QSharedPointer<IReciverDevice> dev,
               QSharedPointer<IDemodulator> dem,
               size_t dataSize = MODES_DATA_LEN + MODES_FULL_LEN_OFFS);

    ~DataWorker() override;

    void setLogger(ILogger*) override;
    void setReciverDevice(QSharedPointer<IReciverDevice> dev) override;
    void setDemodulator(QSharedPointer<IDemodulator> dem) override;
    void abortExec() override;
    void setTimeout(uint64_t msleep) override;
public slots:
    void exec() override;
};

#endif // DATAWORKER_H
