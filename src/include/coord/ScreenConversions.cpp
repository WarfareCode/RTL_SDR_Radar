#include "ScreenConversions.h"


// перевод экранных  координат в прямоугольную систему координат
QPointF ScreenConversions::screenToDecart(const QPointF &screenCenter , const QPointF &xy)
{
    return QPointF( xy.x() - screenCenter.x() ,  - xy.y() + screenCenter.y() );
}

//экранные в полярные
SPolarCoord ScreenConversions::screenToPolar(const QPointF &screenCenter ,const QPointF &xy)
{
    QPointF p = screenToDecart(screenCenter,xy);
    return decartToPolar(p);
}

// перевод прямоугольных координат в экранные
QPointF ScreenConversions::decartToScreen(const QPointF &screenCenter, const QPointF &xy)
{
    return QPointF( xy.x() + screenCenter.x() ,  - xy.y() + screenCenter.y() );
}

//декартоовый в полярные
SPolarCoord ScreenConversions::decartToPolar(const QPointF &p, double toNorth)
{
    //с поправкой на север и угол в положительную сторону
    double rtAngl = - atan2(p.y(), p.x()) / M_PI * 180.0 + toNorth;
    rtAngl = (rtAngl < 0) ? rtAngl + 360 : rtAngl;   //Без этого диапазон от 0...180 и -1...-180
    return SPolarCoord(rtAngl,hypot(p.x(), p.y()));
}

//полярные в декартовые
QPointF ScreenConversions::polarToDecart(const SPolarCoord &plr)
{
    return QPointF(plr.range * cos((plr.phi) * M_PI / 180.0 - M_PI_2),
                   -plr.range * sin((plr.phi) * M_PI / 180.0 - M_PI_2));
}

//полярные в экранные
QPointF ScreenConversions::polarToScreen(const QPointF &screenCenter ,const SPolarCoord &plr)
{
    QPointF p = polarToDecart(plr);
    return decartToScreen(screenCenter, p);
}

//полярные в экранные
QPointF ScreenConversions::polarToScreen(const QPointF &screenCenter ,qreal angl, qreal r)
{
    return polarToScreen(screenCenter, SPolarCoord(angl,r));
}
