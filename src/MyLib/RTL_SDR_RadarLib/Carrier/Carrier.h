#ifndef CARRIER_H
#define CARRIER_H

#include "carrier_global.h"
#include "ICarrierClass.h"
#include "NavigationSystem.h"
#include "NullCarrier.h"
#include "NullNavigationSystem.h"

class CARRIERSHARED_EXPORT Carrier: public ICarrierClass
{

    QSharedPointer<INavigationSystem> _ptrNavigation;

public:
     Carrier(bool isNavigation);
    ~Carrier() override;

     //set/get координат
     void setGeoCoord(const Position &) override{}
     Position getGeoCoord() override;

     //set/get курс
     void setCourse(double)override{}
     double getCourse() override;

     //set/get скорости
     void setSpeed(double)override{}
     double getSpeed() override;

     const QSharedPointer<INavigationSystem> getNavigationSystemMain() override;

};

#endif
