#ifndef IDATACONTROLLER_H
#define IDATACONTROLLER_H

#include <stdint.h>

#include <QSharedPointer>

#include "ILogger.h"
#include "IReciverDevice.h"
#include "IDemodulator.h"
#include "dsp/IDSP.h"
#include "INetworkWorker.h"

enum class  TYPE_WORKER
{
    DATA_STREAM = 0,
    DATA_TO_NETWORK
};

class IDataController
{
public:
    virtual ~IDataController(){}

    virtual void setLogger(ILogger*) = 0;
    virtual void setReciverDevice(QSharedPointer<IReciverDevice>) = 0;
    virtual void setDemodulator(QSharedPointer<IDemodulator>) = 0;
    virtual void setDSP(QSharedPointer<IDSP>) = 0;

    virtual void run() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() = 0;
};
#endif // IDATACONTROLLER_H
