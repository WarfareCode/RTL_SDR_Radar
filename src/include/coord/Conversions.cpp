#include "Conversions.h"

#include <cmath>
#include <QtDebug>

const qreal pi = 3.141592653589793238462643383;
const qreal Conversions::A_EARTH = 6378137;
const qreal flattening = 1.0/298.257223563;
const qreal NAV_E2 = (2.0-flattening)*flattening;
const qreal deg2rad = pi/180.0;
const qreal rad2deg = 180.0/pi;

//static
QVector3D Conversions::lla2xyz(qreal wlat, qreal wlon, qreal walt)
{
    QVector3D toRet;

    //The reference code tests for wlon > 360.0 for some reason. I'm assuming that's a bug
    if (wlat < -90.0 || wlat > 90.0 || wlon < -180.0 || wlon > 180.0)
    {
        qDebug() << "Lat/Lon out of range" << wlat << wlon;
        return toRet;
    }

    qreal slat = sin(wlat*deg2rad);
    qreal clat = cos(wlat*deg2rad);
    qreal r_n = A_EARTH/sqrt(1.0 - NAV_E2*slat*slat);

    toRet.setX((r_n + walt)*clat*cos(wlon*deg2rad));
    toRet.setY((r_n + walt)*clat*sin(wlon*deg2rad));
    toRet.setZ((r_n*(1.0 - NAV_E2) + walt)*slat);

    return toRet;
}

QVector3D Conversions::lla2xyz(const Position & lla)
{
    return Conversions::lla2xyz(lla.latitude(),
                                lla.longitude(),
                                lla.altitude());
}

Position Conversions::xyz2lla(const QVector3D & v)
{
    return Conversions::xyz2lla(v.x(),v.y(),v.z());
}

Position Conversions::xyz2lla(qreal x, qreal y, qreal z)
{
    Position toRet;
    toRet.setAltitude(0.0);

    if (x == 0.0 && y == 0.0 && z == 0.0)
    {
        qDebug() << "Error: XYZ at center of the earth";
        return toRet;
    }

    if (x == 0.0 && y == 0.0)
        toRet.setLongitude(0.0);
    else
        toRet.setLongitude(atan2(y,x)*rad2deg);

    qreal rhosqrd = x*x + y*y;
    qreal rho = sqrt(rhosqrd);
    qreal templat = atan2(z,rho);
    qreal tempalt = sqrt(rhosqrd + z*z) - A_EARTH;
    qreal rhoerror = 1000.0;
    qreal zerror = 1000.0;

    while (abs(rhoerror) > 0.000001 || abs(zerror) > 0.000001)
    {
        qreal slat = sin(templat);
        qreal clat = cos(templat);
        qreal q = 1.0 - NAV_E2*slat*slat;
        qreal r_n = A_EARTH/sqrt(q);
        qreal drdl = r_n*NAV_E2*slat*clat/q;

        rhoerror = (r_n + tempalt)*clat - rho;
        zerror = (r_n*(1.0 - NAV_E2) + tempalt)*slat - z;

        qreal aa = drdl*clat - (r_n + tempalt)*slat;
        qreal bb = clat;
        qreal cc = (1.0 - NAV_E2)*(drdl*slat + r_n*clat);
        qreal dd = slat;

        qreal invdet = 1.0/(aa*dd - bb*cc);
        templat = templat - invdet*(dd*rhoerror - bb*zerror);
        tempalt = tempalt - invdet*(-1*cc*rhoerror + aa*zerror);
    }

    toRet.setLatitude(templat*rad2deg);
    toRet.setAltitude(tempalt);
    return toRet;
}

QVector3D Conversions::xyz2enu(const QVector3D &xyz, qreal reflat, qreal reflon, qreal refalt)
{
    QVector3D refxyz = Conversions::lla2xyz(reflat,reflon,refalt);
    QVector3D diffxyz = xyz - refxyz;

    QTransform R1 = Conversions::rot(90.0 + reflon,3);
    QTransform R2 = Conversions::rot(90.0-reflat,1);
    QTransform R = R2*R1;

    //MAKE THIS MATRIX MULTIPLY
    qreal x = R.m11()*diffxyz.x() + R.m12()*diffxyz.y() + R.m13()*diffxyz.z();
    qreal y = R.m21()*diffxyz.x() + R.m22()*diffxyz.y() + R.m23()*diffxyz.z();
    qreal z = R.m31()*diffxyz.x() + R.m32()*diffxyz.y() + R.m33()*diffxyz.z();

    QVector3D enu(x,y,z);

    return enu;
}

QVector3D Conversions::xyz2enu(const QVector3D &xyz, const Position &refLLA)
{
    return Conversions::xyz2enu(xyz,
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

QVector3D Conversions::xyz2enu(qreal x, qreal y, qreal z, qreal reflat, qreal reflon, qreal refalt)
{
    return Conversions::xyz2enu(QVector3D(x,y,z),
                                reflat,
                                reflon,
                                refalt);
}

QVector3D Conversions::xyz2enu(qreal x, qreal y, qreal z, const Position &refLLA)
{
    return Conversions::xyz2enu(QVector3D(x,y,z),
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

QVector3D Conversions::enu2xyz(const QVector3D & enu, qreal reflat, qreal reflon, qreal refalt)
{
    const QTransform R1 = Conversions::rot(90.0 + reflon,3);
    const QTransform R2 = Conversions::rot(90.0 - reflat,1);
    const QTransform R = R2*R1;

    const QTransform invR = R.inverted();
    if (invR.isIdentity())
    {
        qDebug() << "Failed to invert rotation matrix --- did you enter a bad lat,lon,or alt?";
        return enu;
    }

    qreal x = invR.m11()*enu.x() + invR.m12()*enu.y() + invR.m13()*enu.z();
    qreal y = invR.m21()*enu.x() + invR.m22()*enu.y() + invR.m23()*enu.z();
    qreal z = invR.m31()*enu.x() + invR.m32()*enu.y() + invR.m33()*enu.z();

    QVector3D diffxyz(x,y,z);

    QVector3D refxyz = Conversions::lla2xyz(reflat,reflon,refalt);

    return (diffxyz + refxyz);
}

QVector3D Conversions::enu2xyz(const QVector3D & enu, const Position & refLLA)
{
    return Conversions::enu2xyz(enu,
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

QVector3D Conversions::enu2xyz(qreal east, qreal north, qreal up, qreal reflat, qreal reflon, qreal refalt)
{
    return Conversions::enu2xyz(QVector3D(east,north,up),
                                reflat,
                                reflon,
                                refalt);
}

QVector3D Conversions::enu2xyz(qreal east, qreal north, qreal up, const Position & refLLA)
{
    return Conversions::enu2xyz(QVector3D(east,north,up),
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

Position Conversions::enu2lla(const QVector3D & enu, qreal reflat, qreal reflon, qreal refalt)
{
    QVector3D xyz = Conversions::enu2xyz(enu,reflat,reflon,refalt);
    return Conversions::xyz2lla(xyz);
}

Position Conversions::enu2lla(const QVector3D & enu, const Position & refLLA)
{
    return Conversions::enu2lla(enu,
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

Position Conversions::enu2lla(qreal east, qreal north, qreal up, qreal reflat, qreal reflon, qreal refalt)
{
    return Conversions::enu2lla(QVector3D(east,north,up),
                                reflat,
                                reflon,
                                refalt);
}

Position Conversions::enu2lla(qreal east, qreal north, qreal up, const Position & refLLA)
{
    return Conversions::enu2lla(QVector3D(east, north, up),
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

QVector3D Conversions::lla2enu(qreal lat, qreal lon, qreal alt, qreal reflat, qreal reflon, qreal refalt)
{
    QVector3D xyz = Conversions::lla2xyz(lat,lon,alt);
    QVector3D enu = Conversions::xyz2enu(xyz,reflat,reflon,refalt);

    return enu;
}

QVector3D Conversions::lla2enu(qreal lat, qreal lon, qreal alt, const Position & refLLA)
{
    return Conversions::lla2enu(lat,lon,alt,
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

QVector3D Conversions::lla2enu(const Position &lla, qreal reflat, qreal reflon, qreal refalt)
{
    return Conversions::lla2enu(lla.latitude(),
                                lla.longitude(),
                                lla.altitude(),
                                reflat,
                                reflon,
                                refalt);
}

QVector3D Conversions::lla2enu(const Position & lla, const Position & refLLA)
{
    return Conversions::lla2enu(lla.latitude(),
                                lla.longitude(),
                                lla.altitude(),
                                refLLA.latitude(),
                                refLLA.longitude(),
                                refLLA.altitude());
}

qreal Conversions::degreesLatPerMeter(const qreal latitude)
{
    const qreal latRad = latitude * (pi / 180.0);
    qreal meters = 111132.954 - 559.822 * cos(2.0 * latRad) + 1.175 * cos(4.0 * latRad);
    return 1.0 / meters;
}

qreal Conversions::degreesLonPerMeter(const qreal latitude)
{
    const qreal latRad = latitude * (pi / 180.0);
    qreal meters = (pi * A_EARTH * cos(latRad)) / (180.0 * sqrt(1.0 - NAV_E2 * pow(sin(latRad), 2.0)));
    return 1.0 / meters;
}

QTransform Conversions::rot(qreal angle, quint32 axis)
{
    QTransform toRet;

    qreal cang = cos(angle*deg2rad);
    qreal sang = sin(angle*deg2rad);

    switch(axis)
    {
    default:
    case 1:
        toRet.setMatrix(1.0,0.0,0.0,0.0,cang,sang,0.0,-1*sang,1.0);
        break;

    case 2:
        toRet.setMatrix(cang,0.0,-1*sang,0.0,1.0,0.0,sang,0.0,cang);
        break;

    case 3:
        toRet.setMatrix(cang,sang,0.0,-1*sang,cang,0.0,0.0,0.0,1.0);
        break;
    }

    return toRet;
}

void Conversions::test()
{
    Position byu1(QPointF(-111.649253,40.249707),1423);
    QVector3D xyz = Conversions::lla2xyz(byu1);
    Position byu2 = Conversions::xyz2lla(xyz);

    if (qAbs(byu2.longitude() - byu1.longitude()) > 0.001 ||
            qAbs(byu2.latitude() - byu1.latitude()) > 0.001 ||
            qAbs(byu2.altitude() - byu1.altitude()) > 1.0)
        qDebug() << "Failed LLA -> XYZ -> LLA";
    else
        qDebug() << "Passed LLA -> XYZ -> LLA";

    QVector3D enu1(5,5,0);
    Position byu3 = Conversions::enu2lla(enu1,byu1);
    QVector3D enu3 = Conversions::lla2enu(byu3,byu1);

    if ((enu3 - enu1).length() > 0.3)
        qDebug() << "Failed LLA -> ENU -> LLA -> ENU";
    else
        qDebug() << "Passed LLA -> ENU -> LLA -> ENU";

    qDebug() << degreesLatPerMeter(15.0);
    qDebug() << degreesLonPerMeter(15.0);
}

bool Conversions::isValidGeoCoord(const Position  &p)
{
    return((p.latitude() > -90.0 && p.latitude() < 90.0) &&
           (p.longitude() > -180 && p.longitude() < 180));
}

bool Conversions::geoCoordToString(const Position &p, QString &lonStr, QString &latStr)
{
    if(!isValidGeoCoord(p))
    {
        latStr = QString::fromLocal8Bit("--° --' --'' --");
        lonStr = QString::fromLocal8Bit("--° --' --'' --");
        return false;
    }

    double koef = 1.0;
    double K = 60.0;
    QString strNS("N");
    int lat = p.latitude();
    if(lat < 0)
    {
        koef = -1.0;
        lat = -lat;
        strNS = "S";
    }
    double minut = (koef * p.latitude() - static_cast<double>(lat)) * K;
    double second = (minut - static_cast<int>(minut)) * K;

    QString strWE("E");
    int lon = p.longitude();
    koef = 1.0;
    if(lon < 0)
    {
        lon = -lon;
        strWE = "W";
        koef = -1.0;
    }
    double minut1 = (koef * p.longitude() - static_cast<double>(lon)) * K;
    double second1 = ( minut1 - static_cast<int>(minut1)) * K;

    //широта
   latStr = QString::fromLocal8Bit("%1° %2' %3'' %4")
                                   .arg(static_cast<int>(lat))
                                   .arg(static_cast<int>(minut))
                                   .arg(static_cast<int>(second))
                                   .arg(strNS);
    //долгота
    lonStr = QString::fromLocal8Bit("%1° %2' %3'' %4")
                                    .arg(static_cast<int>(lon))
                                    .arg(static_cast<int>(minut1))
                                    .arg(static_cast<int>(second1))
                                    .arg(strWE);
    return true;
}

//**********************************************************************
//**  Возвращает длину дуги большого круга между двумя точками
//**  с координатами (AL0,AB0) и (AL1,AB1)
double Conversions::getDistance(Position p0,Position p1,double AR)
{
    float f;
    QVarLengthArray<double> XYZ0(3),XYZ1(3);

    double D2;
    // ВЫчисляем длину хорды
    XYZ0 = LBToXYZ(p0);
    XYZ1 = LBToXYZ(p1);
    D2 = XYZ0.data()[0]*XYZ1.data()[0] + XYZ0.data()[1]*XYZ1.data()[1] + XYZ0.data()[2]*XYZ1.data()[2];
    if (fabs(D2) < 1.0)
        f = AR*acos(D2);
    else
        f= M_PI*AR;
    return f;
}


//**********************************************************************
//**  возвращает геоцентрические координаты X,Y,Z по долготе и широте
QVarLengthArray<double> Conversions::LBToXYZ(Position p)
{
    double Rxy, Sn, Cs, Teta;
    QVarLengthArray<double> res(3);
    Teta = deg2rad*p.latitude();
    sincos(Teta, &Sn, &Rxy);
    res.data()[2] = Sn;
    Teta = deg2rad*p.longitude();
    sincos(Teta, &Sn, &Cs);
    res.data()[0] = Rxy*Cs;
    res.data()[1] = Rxy*Sn;
    return res;
}


//**********************************************************************
//**  возвращает географические координаты L и B по геоцентрическим
Position Conversions::XYZToLB(QVarLengthArray<double> p0)
{
    double R;
    Position res;

    R = sqrt(p0.data()[0]*p0.data()[0]+p0.data()[1]*p0.data()[1]+p0.data()[2]*p0.data()[2]);
    res.setLatitude(rad2deg*(asin(p0.data()[2]/R)));

    if (p0.data()[1] > 0)
        res.setLongitude(rad2deg*(M_PI_2-atan(p0.data()[0]/p0.data()[1])));
    else
    {
        if (p0.data()[1]==0)
            res.setLongitude(0);
        else
            res.setLongitude(rad2deg*(-M_PI_2-atan(p0.data()[0]/p0.data()[1])));
    }
    return res;
}

//**********************************************************************
//**  возвращает место (координаты  L и B) по пеленгу и расстоянию на cфере
Position Conversions::getPlace(Position p0, double APeleng, double ADistance, double AR)
{
    Position Result;
    double L1, B1;
    QVarLengthArray<double> P(3);

    L1 = p0.longitude();
    B1 = p0.latitude()+rad2deg*(ADistance/AR);
    if (B1 > 90.0)
    {
        B1 = 180.0-B1;
        L1 = p0.longitude()+180.0;
        if (L1 > 180.0)
            L1 = p0.longitude() - 180.0;
    }

    P = Rotate(LBToXYZ(Position(L1,B1)),LBToXYZ(p0),APeleng);

    Result = XYZToLB(P);
    return Result;
}


//**********************************************************************
//**   вращает вектор AV относительно вектора AAxis на угол AAlpha
QVarLengthArray<double> Conversions::Rotate(QVarLengthArray<double> AV ,QVarLengthArray<double> AAxis, double AAlpha)
{
    double CosA, SinA, C1, C2, C3, DV;
    QVarLengthArray<double> res(3);

    double Teta = -deg2rad*(AAlpha);
    SinA = sin(Teta);
    CosA = cos(Teta);
    //длина вектора AAxis
    DV = sqrt((AAxis.data()[0])*(AAxis.data()[0])+(AAxis.data()[1])*(AAxis.data()[1])+(AAxis.data()[2])*(AAxis.data()[2]));
    //Направляющте косинусы
    C1 = AAxis.data()[0]/DV;
    C2 = AAxis.data()[1]/DV;
    C3 = AAxis.data()[2]/DV;

    //Вращения по X,Y,Z
    res.data()[0] = CosA*AV.data()[0]+(1-CosA)*(C1*C1*AV.data()[0]+C1*C2*AV.data()[1]+C1*C3*AV.data()[2])+
            SinA*(-C3*AV.data()[1]+C2*AV.data()[2]);

    res.data()[1] = CosA*AV.data()[1]+(1-CosA)*(C2*C1*AV.data()[0]+C2*C2*AV.data()[1]+C2*C3*AV.data()[2])+
            SinA*(C3*AV.data()[0]-C1*AV.data()[2]);

    res.data()[2] = CosA*AV.data()[2]+(1-CosA)*(C3*C1*AV.data()[0]+C3*C2*AV.data()[1]+C3*C3*AV.data()[2])+
            SinA*(-C2*AV.data()[0]+C1*AV.data()[1]);

    return res;
}



//**********************************************************************
//**  возвращает пеленг точки (AL1,AB1) относительно (AL0,AB0)
double Conversions::LBToPeleng(Position p0, Position p1)
{
    long double a,b,c,s;
    double res;
    a = getDistance(p1, Position(0,90),1);
    b = getDistance(p0, Position(0,90),1);
    c = getDistance(p0,p1,1);

    s =(a+b+c)/2;

    s = sin(s)*sin(s-a)/sin(b)/sin(c);
    if (s >= 1)
        res = 0.0;
    else
    {
        if (s <= 0.0)
        {
            if (p1.latitude() <= p0.latitude())
                res = 180.0;
            else
                res = 0.0;
        }
        else
        {
            while (p1.longitude() < p0.longitude())
                p1.setLongitude(p1.longitude() + 360.0);
            if (p1.longitude() - p0.longitude() <= 180.0)
                res = 2.0*rad2deg*(atan((sqrt(1.0-s)/sqrt(s))));
            else
                res = 360.0 - 2.0*rad2deg*(atan((sqrt(1.0-s)/sqrt(s))));
        }
    }
    return res;
}
