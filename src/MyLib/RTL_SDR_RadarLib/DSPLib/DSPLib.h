#ifndef DSPLIB_H
#define DSPLIB_H

#include "dsplib_global.h"
#include "dsp/IDSP.h"
#include <fftw3.h>

class DSPLIBSHARED_EXPORT DSPLib: public IDSP
{
    std::complex<float> *inbuf, *outbuf;
    fftw_plan plan;
    const int N = 2048;

public:
    DSPLib();
    ~DSPLib() override;
    SrcDataAdc makeFFT(const QVector<uint8_t>& vector) override;
    SrcDataAdc makeMagnitude(const QVector<uint8_t>& vector) override;
    SrcDataAdc makeAll(const QVector<uint8_t>& vector)override;
};

#endif // DSPLIB_H
