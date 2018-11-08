#ifndef IOBJECT
#define IOBJECT

#include "coord/Position.h"


enum class OBJECT_STATE
{
    NEW_OBJECT = 0,
    UPDATE_OBJECT,
    DELETE_OBJECT,
    NOT_UPDATE_OBJECT,
};

class IObject
{
public:
    virtual ~IObject() {}

    //id
    virtual void setId(uint64_t) = 0 ;
    virtual uint64_t getId() = 0;

    //set/get время регистрации
    virtual void setDateTimeStart(const QDateTime &dt) = 0;
    virtual QDateTime getDateTimeStart() const = 0;

    //set/get время последнего обнаружения
    virtual void setDateTimeStop(const QDateTime &dt) = 0;
    virtual QDateTime getDateTimeStop() const = 0;

    //set/get время регистрации в мсек с начала эпохи
    virtual void setMSecStart(const qint64 dt) = 0;
    virtual qint64 getMSecStart() const = 0;

    //set/get время последнего обнаружения в мсек с начала эпохи
    virtual void setMSecStop(const qint64 dt) = 0;
    virtual qint64 getMSecStop() const = 0;

    //set/get состояние объекта
    virtual void setObjectState(OBJECT_STATE state) = 0;
    virtual OBJECT_STATE getObjectState() = 0;

    // имя объекта
    virtual void setObjectName(const QString& str) = 0;
    virtual QString getObjectName() = 0;

    //пеленг
    virtual void setAzimuth(double azim) = 0;
    virtual double getAzimuth() const = 0;

    //угол места
    virtual void setElevation(double elev) = 0;
    virtual double getElevation() = 0;

    //set/get дистанции
    virtual void setDistance_KM(const qreal dist) = 0;
    virtual double getDistance_KM() = 0;

    virtual void setDistance_M(const qreal dist) = 0;
    virtual double getDistance_M() = 0;

    //set/get гео координаты
    virtual void setGeoCoord(const Position &gp) = 0;
    virtual Position getGeoCoord() const = 0;

    virtual bool isValidGeoCoord() = 0;

    //set/get скорость
    virtual void setSpeed(const double value) = 0;
    virtual double getSpeed() const = 0;

    //курс
    virtual void setCourse(double crs) = 0;
    virtual double getCourse() = 0;

    //установить объект текущим или нет
    virtual void setSelectObject(bool value) = 0;
    virtual bool isSelectedObject() const = 0;

    virtual void setInUse(bool value) = 0;
    virtual bool getInUse() = 0;

    virtual void resetObjectData() = 0;
};
#endif // IOBJECT

