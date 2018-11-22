#ifndef DATACONTROLLER_H
#define DATACONTROLLER_H

#include <QThread>

#include "datacontroller_global.h"
#include "../../../include/IDataController.h"
#include "../../../include/IPoolObject.h"

#include "DataWorker.h"
#include "DataWorkerImit.h"

class DATACONTROLLERSHARED_EXPORT DataController : public IDataController
{
    IWorker* _worker = nullptr;
    QThread* _dataThread = nullptr;

public:
    explicit DataController(QSharedPointer<IReciverDevice> dev,
                            QSharedPointer<IDemodulator> dem);
    ~DataController() override;

    uint8_t* getDataToChart() override;

    void run() override;
    void stop() override;
    bool isRunning() override;
};

#endif // DATACONTROLLER_H
