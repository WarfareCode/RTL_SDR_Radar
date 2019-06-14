#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <stdint.h>
#include <QString>
#include <math.h>

class Aircraft
{
    static constexpr char SIZE_TEXT = 9;
    const uint32_t VALUE_LSB = 100;
    const double LON_VALUE_LSB = 360.0 / pow(2, 31);
    const double LAT_VALUE_LSB = 180.0 / pow(2, 31);

    uint32_t _icao;      /* ICAO address */
    char _flight[SIZE_TEXT];     /* Flight number */
    float _altitude;       /* Altitude */
    float _speed;          /* Velocity computed from EW and NS components. */
    float _course;          /* Angle of flight. */
    int64_t _seen;        /* Time at which the last packet was received. */
    uint64_t _messages;      /* Number of Mode S messages received. */
    /* Encoded latitude and longitude as extracted by odd and even
     * CPR encoded messages. */
    int _odd_cprlat;
    int _odd_cprlon;
    int _even_cprlat;
    int _even_cprlon;
    double _lat;
    double _lon;
    /* Coordinated obtained from CPR encoded data. */
    int64_t _odd_cprtime;
    int64_t _even_cprtime;

public:
    Aircraft(uint32_t icao);
    ~Aircraft() = default;

    uint32_t getICAO() const { return _icao; }

    void setAltitude(float val){ _altitude = val; }
    float getAltitude() const { return  _altitude; }

    void setSpeed(float val) { _speed = val; }
    float getSpeed() const { return _speed; }

    void setCourse( float val ) { _course = val; }
    float getCourse() const { return _course; }

    void setSeenTime(int64_t val) { _seen = val; }
    int64_t getSeenTime() const { return _seen; }

    void incNumberMsg() { ++_messages; }
    uint64_t getNumberMsg() const { return _messages; }

    void setFlightInfo(char* val);
    QString getFlightInfo() const;

    void setLatitude( double lat) { _lat = lat; }
    double getLatitude() const { return _lat; }

    void setLongitude( double lon) { _lon = lon; }
    double getLongitude() const { return  _lon; }

    void setOddCprLat( int val ) { _odd_cprlat = val; }
    int  getOddCprLat() const { return _odd_cprlat; }

    void setOddCprLon( int val ) { _odd_cprlon = val; }
    int  getOddCprLon() const { return _odd_cprlon; }

    void setEvenCprLat( int val ) { _even_cprlat = val; }
    int  getEvenCprLat() const { return _even_cprlat; }

    void setEvenCprLon( int val ) { _even_cprlon = val; }
    int  getEvenCprLon() const { return _even_cprlon; }

    void setOddCprTime(int64_t val) { _odd_cprtime = val; }
    int64_t getOddCprTime() const { return _odd_cprtime; }

    void setEvenCprTime(int64_t val) { _even_cprtime = val; }
    int64_t getEvenCprTime() const { return _even_cprtime; }

    QString toString();

    QByteArray serialize();
    bool unserialize(QByteArray array);
};

#endif // AIRCRAFT_H
