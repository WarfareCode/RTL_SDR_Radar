#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

#include <QSharedPointer>

#include "ICarrierClass.h"
#include "NullCarrier.h"

class ServiceLocator
{
    static QSharedPointer<ICarrierClass> _carrier;
public:
    static QSharedPointer<ICarrierClass> getCarrier();
    static void provide(QSharedPointer<ICarrierClass> carrier);
};


#endif // SERVICELOCATOR_H
