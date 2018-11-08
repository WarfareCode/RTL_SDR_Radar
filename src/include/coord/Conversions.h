#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <QTransform>
#include <QVector3D>
#include <QPointF>
#include <QVarLengthArray>
#include "Position.h"

class  Conversions
{
public:
    static QVector3D lla2xyz(qreal wlat, qreal wlon, qreal walt);
    static QVector3D lla2xyz(const Position &lla);
    static Position xyz2lla(const QVector3D &);
    static Position xyz2lla(qreal x, qreal y, qreal z);

    static QVector3D xyz2enu(const QVector3D & xyz, qreal reflat, qreal reflon, qreal refalt);
    static QVector3D xyz2enu(const QVector3D & xyz, const Position & refLLA);
    static QVector3D xyz2enu(qreal x, qreal y, qreal z, qreal reflat, qreal reflon, qreal refalt);
    static QVector3D xyz2enu(qreal x, qreal y, qreal z, const Position & refLLA);
    static QVector3D enu2xyz(const QVector3D & enu, qreal reflat, qreal reflon, qreal refalt);
    static QVector3D enu2xyz(const QVector3D & enu, const Position & refLLA);
    static QVector3D enu2xyz(qreal east, qreal north, qreal up, qreal reflat, qreal reflon, qreal refalt);
    static QVector3D enu2xyz(qreal east, qreal north, qreal up, const Position &refLLA);

    static Position enu2lla(const QVector3D &enu, qreal reflat, qreal reflon, qreal refalt);
    static Position enu2lla(const QVector3D &enu, const Position &refLLA);
    static Position enu2lla(qreal east, qreal north, qreal up, qreal reflat, qreal reflon, qreal refalt);
    static Position enu2lla(qreal east, qreal north, qreal up, const Position & refLLA);
    static QVector3D lla2enu(qreal lat, qreal lon, qreal alt, qreal reflat, qreal reflon, qreal refalt);
    static QVector3D lla2enu(qreal lat, qreal lon, qreal alt, const Position & refLLA);
    static QVector3D lla2enu(const Position & lla, qreal reflat, qreal reflon, qreal refalt);
    static QVector3D lla2enu(const Position & lla, const Position & refLLA);

    static qreal degreesLatPerMeter(const qreal latitude);
    static qreal degreesLonPerMeter(const qreal latitude);

    static QTransform rot(qreal angle, quint32 axis);

    // Возвращает длину дуги большого круга между двумя точками с координатами p0 и p1 в метрах
    static double getDistance(Position p0,Position p1, double AR = A_EARTH);
    //возвращает геоцентрические координаты X,Y,Z по долготе и широте
    static QVarLengthArray<double> LBToXYZ(Position p);
    static Position XYZToLB(QVarLengthArray<double> p0);
    static Position getPlace(Position p0, double APeleng, double ADistance, double AR = A_EARTH);
    static QVarLengthArray<double> Rotate(QVarLengthArray<double> AV ,QVarLengthArray<double> AAxis, double AAlpha);
    static double LBToPeleng(Position p0, Position p1);

    static bool isValidGeoCoord(const Position &p);
    static bool geoCoordToString(const Position& p, QString &lonStr, QString &latStr);
private:
    static const qreal A_EARTH;
    static void test();

};

#endif // CONVERSIONS_H
