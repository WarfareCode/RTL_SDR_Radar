#include <QDateTime>
#include <QDataStream>
#include <QByteArray>
#include <QDebug>

#include "Aircraft.h"
#include "StructAircraft.h"

Aircraft::Aircraft(uint32_t icao) : _icao(icao)
{
    _flight[0] = '\0';
    _altitude = 0;
    _speed = 0;
    _course = 0;
    _odd_cprlat = 0;
    _odd_cprlon = 0;
    _odd_cprtime = 0;
    _even_cprlat = 0;
    _even_cprlon = 0;
    _even_cprtime = 0;
    _lat = -200.0;
    _lon = -400.0;
    _seen = QDateTime::currentMSecsSinceEpoch();
    _messages = 0;
}

void Aircraft::setFlightInfo(char *val)
{
    if(val == nullptr)
        return;

    memcpy(_flight, val, sizeof(_flight));
}

QString Aircraft::getFlightInfo() const
{
    return QString(_flight);
}

QString Aircraft::toString()
{
    QString str;
    str.append( QString("+++++++++++++++++++++++++++++++++++++\n"));
    str.append( QString("ICAO: %1\n").arg(_icao,6,16));
    str.append( QString("Flight number: %1\n").arg(QString(_flight)));
    str.append( QString("Altitude: %1\n").arg(_altitude));
    str.append( QString("Speed: %1\n").arg(_speed));
    str.append( QString("Course: %1\n").arg(_course));
    str.append( QString("Longitude: %1\n").arg(_lon));
    str.append( QString("Latitude: %1\n").arg(_lat));
    str.append( QString("Time last update: %1\n")
                   .arg(QDateTime::fromMSecsSinceEpoch(_seen).toString("hh:mm:ss.zzz")));
    str.append( QString("Number of Mode S messages received: %1\n").arg(_messages));
    str.append( QString("+++++++++++++++++++++++++++++++++++++\n"));

    return  str;
}

QByteArray Aircraft::serialize()
{
    QByteArray array;
    array.resize(sizeof (StructAircraft));

    StructAircraft a;
    a.icao = _icao;

    memset((char*)a.flight,0,sizeof (a.flight));
    memcpy((char*)a.flight, (char*)_flight, sizeof(a.flight));

    a.altitude = uint32_t(_altitude * VALUE_LSB);
    a.speed = uint32_t(_speed * VALUE_LSB);
    a.course = uint32_t(_course * VALUE_LSB);
    a.lon = int32_t(_lon / LON_VALUE_LSB);
    a.lat = int32_t(_lat / LAT_VALUE_LSB);
    a.seen = _seen;
    a.messages = _messages;

    memcpy(array.data(),(char*)&a,sizeof (StructAircraft));

    return  array;
}

bool Aircraft::unserialize(QByteArray array)
{
    Q_UNUSED(array);
    return false;
}

