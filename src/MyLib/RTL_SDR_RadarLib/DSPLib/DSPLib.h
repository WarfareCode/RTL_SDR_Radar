#ifndef DSPLIB_H
#define DSPLIB_H

#include <QRunnable>
#include <fftw3.h>
#include "dsplib_global.h"
#include "dsp/IDSP.h"

class DSPLIBSHARED_EXPORT DSPLib: public IDSP, public QRunnable
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
    void run() override;
};

#endif // DSPLIB_H
