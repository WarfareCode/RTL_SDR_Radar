#ifndef IMAPCONTROLLER
#define IMAPCONTROLLER

#include <QImage>
#include <QObject>

#include "coord/Conversions.h"
#include "coord/ScreenConversions.h"

enum class FilterType
{
    No = 0,
    Night,
    Circle
};

class IMapController : public QObject
{
    Q_OBJECT
public:
    virtual ~IMapController(){}

    virtual QImage getImageMap( int w,
                                  int h,
                                  Position &center,
                                  double scale,
                                  FilterType type = FilterType::No) = 0;

    virtual QImage getImageMap( int w,
                                  int h,
                                  FilterType type = FilterType::No) = 0;

    //центральная точка в географических координатах
    virtual Position getCenterGeoPoint() = 0;
    virtual void setCenterGeoPoint(const Position &geoCenter) = 0;

    //экранные координаты в полярные c учетом масштаба
    virtual SPolarCoord screenToRealPolar(const QPointF &xy) = 0;
    virtual QPointF realPolarToScreen(const SPolarCoord &plr) = 0;

    virtual Position screenToGeoCoordinates(const QPointF &point) = 0;
    virtual QPointF geoToScreenCoordinates(const Position &sgc) = 0;

    virtual double getDistanceRadarScale_KM() = 0;
    virtual double getDistanceRadarScale_M() = 0;

    //масштаб
    virtual double getScale() = 0;
    virtual void setScale(double scale) = 0;

    //проверка попадает ли точка в область видимости
    virtual bool isVisibleInCurrentScale(double dist) = 0;
};
#endif // IMAPCONTROLLER

