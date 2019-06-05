#ifndef IDEMODULATOR_H
#define IDEMODULATOR_H

#include <QVector>
#include <QRunnable>
#include <QByteArray>

class IPoolObject;

class IDemodulator :  public QRunnable
{
public:
    virtual ~IDemodulator(){}
    virtual bool setDataForDemodulate(const QVector<uint8_t>& vector) = 0;
    virtual QByteArray getRawDump() = 0;
};

#endif // IDEMODULATOR_H
