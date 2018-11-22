#ifndef ICONTAINEROBJECT
#define ICONTAINEROBJECT

#include <QVector>
#include <QSharedPointer>

#include "IObject.h"

typedef  QSharedPointer< QHash<uint64_t, QSharedPointer<IObject> > > pHash;

enum class OBJECT_TYPE
{
    air = 0,
    surface,
};

class IPoolObject
{

public:
    virtual ~IPoolObject(){}
    //создание нового объекта
    virtual QSharedPointer<IObject> createNewObject(OBJECT_TYPE,
                                                    uint64_t,
                                                    QDateTime,
                                                    QDateTime,
                                                    double,
                                                    double) = 0;

    virtual QList<QSharedPointer<IObject>> values() = 0;

    virtual bool isExistsObject(uint64_t id) = 0;
    virtual QSharedPointer<IObject> getObjectByID(uint64_t id) = 0;

    virtual void prepareAllObjectToDelete() = 0;
    virtual void deleteMarkedObjects() = 0;
    virtual void deleteObject(uint64_t id) = 0;

    virtual void lockPool() = 0;
    virtual bool tryLockPool() = 0;
    virtual void unlockPool() = 0;

};
#endif // ICONTAINEROBJECT

