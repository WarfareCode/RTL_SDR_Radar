#ifndef NULLCARRIER_H
#define NULLCARRIER_H

#include <QSharedPointer>

#include "ICarrierClass.h"
#include "NullNavigationSystem.h"

class NullCarrier final : public ICarrierClass
{
    QSharedPointer<INavigationSystem> _ptrNavigation;

public:
    NullCarrier();
    ~NullCarrier();

     //set/get координат
     void setGeoCoord(const Position&){}
     Position getGeoCoord() { return _ptrNavigation->getGeoCoord(); }

     //set/get курс
     void setCourse(double) {}
     double getCourse() { return _ptrNavigation->getCourse(); }

     //set/get скорости
     void setSpeed(double) {}
     double getSpeed() { return _ptrNavigation->getSpeed(); }

     const QSharedPointer<INavigationSystem> getNavigationSystemMain()
     {
         return _ptrNavigation;
     }

     double getAreaDAA() { return 0; }
};

#endif // NULLCORVETTE_H
