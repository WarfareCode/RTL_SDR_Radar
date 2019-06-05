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
    QSharedPointer<INetworkWorker> _net;
    ILogger* _log;

    QMutex _mutex;
    QWaitCondition _condition;
    std::atomic<bool> _abort;

    size_t _dataVectorSize = MODES_DATA_LEN + MODES_FULL_LEN_OFFS;
    QVector<uint8_t> _dataVector;


    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _lastRequest;

    std::atomic<uint32_t> _ms_sleep;
    uint32_t DEFAULT_SLEEP_MS = 2000;

    bool processData();
public:
    DataWorker(QSharedPointer<IReciverDevice> dev,
               QSharedPointer<IDemodulator> dem,
               size_t dataSize = MODES_DATA_LEN + MODES_FULL_LEN_OFFS);

    ~DataWorker() override;

    void setLogger(ILogger* log) override { _log = log; }
    void setReciverDevice(QSharedPointer<IReciverDevice> dev) override { _device = dev;}
    void setDemodulator(QSharedPointer<IDemodulator> dem) override { _demod = dem; }
    void setDSP(QSharedPointer<IDSP> dsp) override { _dsp = dsp; }
    void setNetworkModule(QSharedPointer<INetworkWorker> net) override { _net = net; }

    void abortExec() override { _abort = true; }
    void setTimeout(uint64_t msleep) override { _ms_sleep = msleep; }
public slots:
    void exec() override;
};

#endif // DATAWORKER_H
