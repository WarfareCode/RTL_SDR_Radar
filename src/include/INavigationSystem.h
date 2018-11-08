#ifndef INAVIGATIONSYSTEM
#define INAVIGATIONSYSTEM

#include "coord/Position.h"

class INavigationSystem
{
public:
    virtual ~INavigationSystem(){}
    // курс
    virtual void setCourse(double) = 0;
    virtual double getCourse() = 0;

    //set/get координат
    virtual void setGeoCoord(const Position &pst) = 0;
    virtual Position getGeoCoord() = 0;

    //set/get скорости
    virtual void setSpeed(double val) = 0;
    virtual double getSpeed() = 0;
};
#endif // INAVIGATIONSYSTEM

