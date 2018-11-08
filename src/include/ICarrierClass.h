#ifndef ICARRIERCLASS_H
#define ICARRIERCLASS_H
/************************************************************
 *  Разработчик: Данильченко А.Е.
 ************************************************************/

#include <QObject>
#include <QTime>
#include <QMutex>
#include <memory>

#include "coord/Position.h"
#include "INavigationSystem.h"

class ICarrierClass
{

public:
    virtual ~ICarrierClass(){}

    //set/get координат
    virtual void setGeoCoord(const Position &) = 0;
    virtual Position getGeoCoord() = 0;

    //set/get курс
    virtual void setCourse(double) = 0;
    virtual double getCourse() = 0;

    //set/get скорости
    virtual void setSpeed(double) = 0;
    virtual double getSpeed() = 0;

    //система навигации
    virtual const QSharedPointer<INavigationSystem> getNavigationSystemMain() = 0;

};



#endif
