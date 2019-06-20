#ifndef DATACONTROLLER_H
#define DATACONTROLLER_H

#include <QThread>
#include <memory>

#include "datacontroller_global.h"
#include "IDataController.h"
#include "IPoolObject.h"
#include "IWorker.h"

class DATACONTROLLERSHARED_EXPORT DataController : public IDataController
{
    std::unique_ptr<IWorker> _worker = nullptr;
    QThread* _dataThread = nullptr;

public:
    explicit DataController(QSharedPointer<IReciverDevice> dev,
                            QSharedPointer<IDemodulator> dem,
                            TYPE_WORKER typeWorker = TYPE_WORKER::DATA_STREAM);
    ~DataController() override;

    void run() override;
    void stop() override;
    bool isRunning() override;

    void setLogger(ILogger* log) override;
    void setReciverDevice(QSharedPointer<IReciverDevice> dev) override;
    void setDemodulator(QSharedPointer<IDemodulator> dem) override;
    void setDSP(QSharedPointer<IDSP> dsp) override;
};

#endif // DATACONTROLLER_H
