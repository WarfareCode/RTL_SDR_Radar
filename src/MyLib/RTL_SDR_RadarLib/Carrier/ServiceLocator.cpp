#include "ServiceLocator.h"

QSharedPointer<ICarrierClass> ServiceLocator::_carrier;

QSharedPointer<ICarrierClass> ServiceLocator::getCarrier()
{
    return _carrier;
}

void ServiceLocator::provide(QSharedPointer<ICarrierClass> carrier)
{
   if(carrier.isNull())
        _carrier = QSharedPointer<ICarrierClass>(new NullCarrier());
    else
       _carrier = carrier;

}
