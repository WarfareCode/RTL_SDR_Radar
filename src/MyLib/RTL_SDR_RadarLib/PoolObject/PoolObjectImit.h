#ifndef POOLOBJECTIMIT_H
#define POOLOBJECTIMIT_H


#include "IObject.h"
#include "IPoolObject.h"

typedef  QSharedPointer< QList<QSharedPointer<IObject> > > pList;
class PoolObjectImit : public IPoolObject
{
    pList _container;
public:
    PoolObjectImit();
    ~PoolObjectImit();

    QSharedPointer<IObject> createNewObject(uint64_t id,
                                            uint64_t tstart,
                                            uint64_t tstop,
                                            double azimuth,
                                            double elevation,
                                            double,
                                            double,
                                            double,
                                            double,
                                            double) override final;
    virtual QSharedPointer<IObject> createNullObject() final;

    QList<QSharedPointer<IObject> > values() final;


    bool isExistsObject(uint64_t id) final;
    QSharedPointer<IObject> getObjectByID(uint64_t id) final;

    bool isExistsObjectSecondId(uint16_t) final;
    QSharedPointer<IObject> getObjectBySecondID(uint16_t) final;

    bool isExistsInCarriers(uint32_t id) final;
    bool isExistsInCarrierAndREM( uint32_t idCarrier, uint8_t idREM) final;

    QList<QSharedPointer<IObject>> getListObjectsByCarrier(uint32_t idCarrier) final;
    QSharedPointer<IObject> getObjectInCarrierByREM(uint32_t idCarrier,
                                                    uint8_t idREM);
    void prepareAllObjectToDelete() final;
    void deleteMarkedObjects() final;
    void deleteObject(uint64_t id) final;
    void setTimeActualData(int) final;
};

#endif // POOLOBJECTIMIT_H
