#ifndef NAVIGATIONSYSTEM_H
#define NAVIGATIONSYSTEM_H

#include <QDateTime>

#include "INavigationSystem.h"

class NavigationSystem : public INavigationSystem
{
    //гео координаты
    Position _geoCoord;
    //курс
    double _course;
    //скорость
    double _speed;

public:
    explicit NavigationSystem();

    // курс
    void setCourse(double course) { _course = course; }
    double getCourse() { return _course; }

    //set/get координат
    void setGeoCoord(const Position & geo) { _geoCoord = geo; }
    Position getGeoCoord() { return _geoCoord; }

    //set/get скорости
    void setSpeed(double speed) { _speed = speed; }
    double getSpeed() { return _speed; }

};

#endif // NAVIGATIONSYSTEM_H
