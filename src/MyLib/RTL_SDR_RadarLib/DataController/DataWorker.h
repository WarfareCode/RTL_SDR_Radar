#ifndef DATAWORKER_H
#define DATAWORKER_H

#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "../../../include/IWorker.h"
#include "../../../../import/sdr_dev/include/constant.h"

class DataWorker : public IWorker
{
    Q_OBJECT

    QSharedPointer<IReciverDevice> _device;
    QSharedPointer<IDemodulator> _demod;
    QVector<uint8_t> _dataVector;
    QMutex _mutex;
    QWaitCondition _condition;
    bool _abort = false;

public:
    DataWorker(QSharedPointer<IReciverDevice> dev,
               QSharedPointer<IDemodulator> dem);
    ~DataWorker();

    void setLogger(ILogger*) override;
    void setReciverDevice(QSharedPointer<IReciverDevice> dev) override;
    void setDemodulator(QSharedPointer<IDemodulator> dem) override;
    void abortExec() override;
    void getData() override;
public slots:
    void exec() override;
};

#endif // DATAWORKER_H
