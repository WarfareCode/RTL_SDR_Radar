#ifndef POOLOBJECT_H
#define POOLOBJECT_H

#include "poolobject_global.h"

#include <QSharedPointer>
#include <QDateTime>
#include <QMutex>

#include "IPoolObject.h"
#include "AirObject.h"
#include "../Subject/Subject.h"

class POOLOBJECTSHARED_EXPORT PoolObject : public IPoolObject, public Subject
{
    pHash _container;
    int32_t _timeActualData = 12000;
    QMutex _mutex;

public:
    explicit PoolObject();
    ~PoolObject() override;
    QSharedPointer<IObject> createNewObject(OBJECT_TYPE type,
                                            uint64_t id,
                                            QDateTime tstart,
                                            QDateTime tstop,
                                            double azimuth,
                                            double elevation) override;

    QList<QSharedPointer<IObject> > values() override;


    bool isExistsObject(uint64_t id) override;
    QSharedPointer<IObject> getObjectByID(uint64_t id) override;

    void prepareAllObjectToDelete() override;
    void deleteMarkedObjects() override;
    void deleteObject(uint64_t id) override;

    void lockPool() override { _mutex.lock(); }
    bool tryLockPool() override { return _mutex.tryLock(2);}
    void unlockPool() override { _mutex.unlock(); }
};

#endif // POOLOBJECT_H
