#ifndef IDSP_H
#define IDSP_H

#include "SrcDataAdc.h"

class IDSP
{

public:
    virtual ~IDSP(){}
    virtual SrcDataAdc makeFFT(const QVector<uint8_t>& vector) = 0;
    virtual SrcDataAdc makeMagnitude(const QVector<uint8_t>& vector) = 0;
    virtual SrcDataAdc makeAll(const QVector<uint8_t>& vector) = 0;
};
#endif // IDSP_H
