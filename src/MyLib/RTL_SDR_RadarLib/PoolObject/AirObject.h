#ifndef RTOBJECT_H
#define RTOBJECT_H

#include <QObject>
#include <QGraphicsItem>
#include <QDateTime>

#include "IObject.h"
#include "GraphicsObject.h"

class AirObject : public GraphicsObject, public IObject
{
   Q_OBJECT
public:
    explicit AirObject(uint64_t id,
                      QDateTime tstart,
                      QDateTime tstop,
                      double azimuth,
                      double elevation);

    ~AirObject() override;

    void setId(uint64_t id) override;
    //id из БД
    uint64_t getId() override;

    //set/get время регистрации
    void setDateTimeStart(const QDateTime &dt) override;
    QDateTime getDateTimeStart() const override;

    //set/get время последнего обнаружения
    void setDateTimeStop(const QDateTime &dt) override;
    QDateTime getDateTimeStop() const override;

    //set/get время регистрации в мсек с начала эпохи
    void setMSecStart(const qint64 dt) override;
    qint64 getMSecStart() const override;

    //set/get время последнего обнаружения в мсек с начала эпохи
    void setMSecStop(const qint64 dt) override;
    qint64 getMSecStop() const override;

    //set/get состояние объекта
    void setObjectState(OBJECT_STATE state) override;
    OBJECT_STATE getObjectState() override;

    // имя носителя
    void setObjectName(const QString& str) override;
    QString getObjectName() override;

    //пеленг
    void setAzimuth(double azim) override;
    double getAzimuth() const override;

    //угол места
    void setElevation(double elev) override;
    double getElevation() override;

    //set/get дистанции
    void setDistance_KM(double dist) override;
    double getDistance_KM() override;

    void setDistance_M(double dist) override;
    double getDistance_M() override;

    //set/get гео координаты
    void setGeoCoord(const Position &gp) override;
    Position getGeoCoord() const override;
    bool isValidGeoCoord() override;

    //set/get скорость
    void setSpeed(double value) override;
    double getSpeed() const override;

    //курс
    void setCourse(double crs) override;
    double getCourse() override;

    //установить объект текущим или нет
    void setSelectObject(bool value) override;
    bool isSelectedObject() const override;

    void setInUse(bool value) override;
    bool getInUse() override;

    void resetObjectData() override;


private:

    uint64_t _id = 0;

    QString _nameObject;

    //состояние объекта
    OBJECT_STATE _state = OBJECT_STATE::NEW_OBJECT;
    //
    bool _inUse = true;

    //время когда был зарегестрирован объект
    int64_t _ms_tstart = 0;
    QDateTime  _tstart;

    //время когда последний раз были обновлены данные
    int64_t _ms_tstop = 0;
    QDateTime  _tstop;

    Position _geoCoord;
    //скорость
    double _speed = -1;
    //курс
    double _course = -1;
    //пеленг
    double _azimuth = -1;
    //угол места
    double _elevation = -1;
    //дальность
    double _distance = -1;


signals:

public slots:
};

#endif
