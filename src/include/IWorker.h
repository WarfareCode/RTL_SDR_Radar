#ifndef IWORKER_H
#define IWORKER_H

#include <QObject>
#include <QSharedPointer>

#include "ILogger.h"
#include "IReciverDevice.h"
#include "IDemodulator.h"
#include "dsp/IDSP.h"
#include "INetworkWorker.h"

class IWorker: public QObject
{
   Q_OBJECT

public:
    virtual ~IWorker(){}

    virtual void setLogger(ILogger*) = 0;
    virtual void setReciverDevice(QSharedPointer<IReciverDevice>) = 0;
    virtual void setDemodulator(QSharedPointer<IDemodulator>) = 0;
    virtual void setDSP(QSharedPointer<IDSP>) = 0;
    virtual void abortExec() = 0;
    virtual void setTimeout(uint64_t msleep) = 0;
public slots:
   virtual void exec() = 0;
signals:
    void finished();
};
#endif // IWORKER_H
