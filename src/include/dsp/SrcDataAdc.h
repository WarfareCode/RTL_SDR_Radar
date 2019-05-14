#ifndef SRCDATAADC_H
#define SRCDATAADC_H

#include <stdint.h>
#include <QVector>
#include <complex.h>

struct SrcDataAdc
{
    QVector<std::complex<int8_t>> rawDataVector;
    QVector<std::complex<float>> magnitudeVector;
    QVector<std::complex<float>> fftVector;
};
#endif // SRCDATAADC_H
