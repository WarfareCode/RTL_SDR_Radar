#ifndef NULLNAVIGATIONSYSYTEM_H
#define NULLNAVIGATIONSYSYTEM_H

#include "INavigationSystem.h"

class NullNavigationSystem final : public INavigationSystem
{
public:
    NullNavigationSystem();

    // курс
    void setCourse(double)  {}
    double getCourse()  { return 45.0; }

    //set/get координат
    void setGeoCoord(const Position &)  {}
    Position getGeoCoord() { return Position(30.4652,60.8422); }

    //set/get скорости
    void setSpeed(double) {}
    double getSpeed(){ return 0.0; }
};

#endif // NULLNAVIGATIONSYSYTEM_H
