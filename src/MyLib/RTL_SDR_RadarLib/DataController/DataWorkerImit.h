#ifndef DATAWORKERIMIT_H
#define DATAWORKERIMIT_H

#include "../../../include/IWorker.h"

class DataWorkerImit : public IWorker
{
public:
    DataWorkerImit();

    void setLogger(ILogger*) override;
    void setReciverDevice(QSharedPointer<IReciverDevice>) override;
    void setDemodulator(QSharedPointer<IDemodulator>) override;
    void abortExec() override;
    void getData() override;
public slots:
    void exec() override;
};

#endif // DATAWORKERIMIT_H
