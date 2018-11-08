#include "Carrier.h"
#include <QDebug>

Carrier::Carrier(bool isNavigation)
{
    qDebug()<<"create carrier";
    if(isNavigation)
        _ptrNavigation = QSharedPointer<INavigationSystem>(new NavigationSystem());
    else
        _ptrNavigation = QSharedPointer<INavigationSystem>(new NullNavigationSystem());
}

Carrier::~Carrier()
{
    qDebug()<<"delete carrier";
    _ptrNavigation.clear();
}


Position Carrier::getGeoCoord()
{    
    if(!_ptrNavigation.isNull())
        return _ptrNavigation->getGeoCoord();

    return Position();
}

//получение значения курса
double Carrier::getCourse()
{
    if(!_ptrNavigation.isNull())
        return _ptrNavigation->getCourse();

    return 0.0;
}

//получить скорость
double Carrier::getSpeed()
{

    if(!_ptrNavigation.isNull())
        return _ptrNavigation->getSpeed();

    return 0.0;
}

const QSharedPointer<INavigationSystem> Carrier::getNavigationSystemMain()
{
    return _ptrNavigation;
}
