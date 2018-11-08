#include "AirObject.h"


AirObject::AirObject(uint64_t id,
                     QDateTime tstart,
                     QDateTime tstop,
                     double azimuth,
                     double elevation):
    GraphicsObject(),
    _id(id),
    _ms_tstart(tstart.toUTC().toMSecsSinceEpoch()),
    _tstart(tstart),
    _ms_tstop(tstart.toUTC().toMSecsSinceEpoch()),
    _tstop(tstop),
    _azimuth(azimuth),
    _elevation(elevation)
{

}

AirObject::~AirObject()
{
}

void AirObject::setId(uint64_t id)
{
    _id = id;
}

uint64_t AirObject::getId()
{
    return _id;
}

void AirObject::setDateTimeStart(const QDateTime &dt)
{
    _tstart = dt;
    _ms_tstart = _tstart.toUTC().toMSecsSinceEpoch();
}

QDateTime AirObject::getDateTimeStart() const
{
    return _tstart;
}

void AirObject::setDateTimeStop(const QDateTime &dt)
{
    _tstop = dt;
    _ms_tstop = _tstop.toUTC().toMSecsSinceEpoch();
}

QDateTime AirObject::getDateTimeStop() const
{
    return _tstop;
}

void AirObject::setMSecStart(const qint64 dt)
{
    _ms_tstart = dt;
    _tstart = QDateTime::fromMSecsSinceEpoch(dt).toUTC();
}

qint64 AirObject::getMSecStart() const
{
    return _ms_tstart;
}

void AirObject::setMSecStop(const qint64 dt)
{
    _ms_tstop = dt;
    _tstop = QDateTime::fromMSecsSinceEpoch(dt).toUTC();
}

qint64 AirObject::getMSecStop() const
{
    return _ms_tstop;
}

void AirObject::setObjectState(OBJECT_STATE state)
{
    _state = state;
}

OBJECT_STATE AirObject::getObjectState()
{
    return _state;
}

void AirObject::setObjectName(const QString &str)
{
    _nameObject = str;
}

QString AirObject::getObjectName()
{
    return _nameObject;
}

void AirObject::setAzimuth(double azim)
{
    _azimuth = azim;
}

double AirObject::getAzimuth() const
{
    return _azimuth;
}

//установить угол места
void AirObject::setElevation(double elev)
{
    _elevation = elev;
}

//получить занчение угла места
double AirObject::getElevation()
{
    return _elevation;
}

void AirObject::setDistance_M(double dist)
{
    _distance = dist;
}

double AirObject::getDistance_M()
{
    return _distance;
}

void AirObject::setDistance_KM(double dist)
{
    _distance = dist * 1.0e3;
}

double AirObject::getDistance_KM()
{
    return _distance / 1.0e3;
}

void AirObject::setGeoCoord(const Position &gp)
{
    _geoCoord = gp;
}

Position AirObject::getGeoCoord() const
{
    return _geoCoord;
}

bool AirObject::isValidGeoCoord()
{

    if((_geoCoord.latitude() > -90.0 && _geoCoord.latitude() < 90.0) &&
       (_geoCoord.longitude() > -180.0 && _geoCoord.longitude() < 180.0))
        return true;
    return false;
}

void AirObject::setSpeed(double value)
{
    _speed = value;
}

double AirObject::getSpeed() const
{
    return _speed;
}

void AirObject::setCourse(double crs)
{
    _course = crs;
    setRotateAngle(_course);
}

double AirObject::getCourse()
{
    return _course;
}

void AirObject::setSelectObject(bool value)
{
    _isSelect = value;
}

bool AirObject::isSelectedObject() const
{
    return _isSelect;
}


void AirObject::setInUse(bool value)
{
    _inUse = value;
}

bool AirObject::getInUse()
{
    return _inUse;
}

void AirObject::resetObjectData()
{
    //нумерация в БД
    _id = 0;
    // носитель которому может принадлежать источник
    _nameObject = QString("--");
    //состояние объекта
    _state = OBJECT_STATE::DELETE_OBJECT;
    //экранные координаты
    _screenPos = QPointF(-10,-10);
    //цвет
    _colorIcon = QColor(0,250,0);
    //размер значка
    _sizeIcon = 20;
    //значек объекта
    img.fill(0x0);
    //рисовать id или нет
    _drawId = false;
    //флаг текущего выбранного объекта
    _isSelect = false;
    //
    _inUse = false;

    //время когда запеленговали в первый раз
    _tstart = QDateTime();
    //время когда последний раз были обновлены данные
    _tstop = QDateTime();
    _ms_tstart = 0;
    _ms_tstop = 0;

    _geoCoord = Position(-200,-200);
    //скорость
    _speed = -1;
    //курс
    _course = -1;
    //пеленг
    _azimuth = -1;
    //угол места
    _elevation = -1;
    //дальность
    _distance = -1;
}
