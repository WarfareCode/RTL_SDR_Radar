#ifndef DATAWORKER_H
#define DATAWORKER_H

#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QWaitCondition>
#include <chrono>
#include <atomic>

#include "IWorker.h"
#include "dsp/IDSP.h"
#include "../../../../import/sdr_dev/include/constant.h"

class DataWorker : public IWorker
{
    Q_OBJECT

    QSharedPointer<IReciverDevice> _device;

    size_t _dataVectorSize = MODES_DATA_LEN + MODES_FULL_LEN_OFFS;
    QVector<uint8_t> _dataVector;

protected:
    std::atomic<bool> _abort;
    QMutex _mutex;

    std::chrono::steady_clock::time_point _firstTimeBreakpoint;
    std::chrono::steady_clock::time_point _secondTimeBreakpoint;

    QSharedPointer<IDemodulator> _demod;
    QSharedPointer<IDSP> _dsp;
    ILogger* _log = nullptr;
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

    void abortExec() override { _abort = true; }
    void setTimeout(uint64_t) override {}
public slots:
    void exec() override;
};

#endif // DATAWORKER_H
