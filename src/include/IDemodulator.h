#ifndef IDEMODULATOR_H
#define IDEMODULATOR_H

#include <QVector>

class IPoolObject;

class IDemodulator
{
public:
    virtual ~IDemodulator(){}
    virtual bool demodulate(QVector<uint8_t>& vector,IPoolObject* pool) = 0;
};

#endif // IDEMODULATOR_H
