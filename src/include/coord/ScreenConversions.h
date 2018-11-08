#ifndef SCREENCONVERSIONS_H
#define SCREENCONVERSIONS_H
#include <QPointF>
#include <math.h>

struct SPolarCoord
{
    SPolarCoord(double p,double r):phi(p),range(r){}
    double phi;      //пеленг
    double range;    //дальность
};

class ScreenConversions
{
public:
    ScreenConversions() = delete;
    static QPointF screenToDecart(const QPointF &screenCenter, const QPointF &xy);
    static SPolarCoord screenToPolar(const QPointF &screenCenter, const QPointF &xy);
    static QPointF decartToScreen(const QPointF &screenCenter, const QPointF &xy);
    static SPolarCoord decartToPolar(const QPointF &p, double toNorth = 90.0);
    static QPointF polarToDecart(const SPolarCoord &plr);
    static QPointF polarToScreen(const QPointF &screenCenter, const SPolarCoord &plr);
    static QPointF polarToScreen(const QPointF &screenCenter, qreal angl, qreal r);
};

#endif // SCREENCONVERSIONS_H
