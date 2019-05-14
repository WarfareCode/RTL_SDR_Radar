#include "DSPLib.h"


DSPLib::DSPLib()
{
    inbuf = (std::complex<float>*)fftw_malloc(sizeof(fftw_complex)*N);
    outbuf = (std::complex<float>*)fftw_malloc(sizeof(fftw_complex)*N);
    plan = fftw_plan_dft_1d(N, (fftw_complex*)inbuf, (fftw_complex*)outbuf,
                            FFTW_FORWARD, FFTW_MEASURE);

}

DSPLib::~DSPLib()
{
    fftw_destroy_plan(plan);
    fftw_free(inbuf);
    fftw_free(outbuf);
}

SrcDataAdc DSPLib::makeFFT(const QVector<uint8_t> &vector)
{
    Q_UNUSED(vector);
}

SrcDataAdc DSPLib::makeMagnitude(const QVector<uint8_t> &vector)
{
    Q_UNUSED(vector);
}

SrcDataAdc DSPLib::makeAll(const QVector<uint8_t> &vector)
{
    SrcDataAdc data;
    int size = vector.size();

    if( size > 0)
    {
        if( (size % 2) != 0)
            --size;

        data.rawDataVector.resize(size / 2);
        data.fftVector.resize(size / 2 );
        for(int i = 0, j = 0; i < N; i +=2, ++j )
        {
            //The magic aligment happens here: we have to change the phase of each next complex sample
            //by pi - this means that even numbered samples stay the same while odd numbered samples
            //get multiplied by -1 (thus rotated by pi in complex plane).
            //This gets us output spectrum shifted by half its size - just what we need to get the output right.
            const float multiplier = (j % 2 == 0 ? 1 : -1);

            std::complex<float> bfr_val(vector[i], vector[i+1]);
            inbuf[j] = (bfr_val - std::complex<float>(127.0, 127.0)) * multiplier;


            data.rawDataVector[j] = std::complex<int8_t>(static_cast<int8_t>(vector.at(i) - INT8_MAX),
                                                         static_cast<int8_t>(vector.at(i+1) - INT8_MAX));
            float rel2 = static_cast<float>(data.rawDataVector[j].real() * data.rawDataVector[j].real());
            float img2 = static_cast<float>(data.rawDataVector[j].imag() * data.rawDataVector[j].imag());
            data.magnitudeVector[j] = sqrtf(rel2+img2);
        }
        fftw_execute(plan);
    }

    for (int i = 0, j = 0; i < N; i++, j+=2)
    {
        double re2 = outbuf[i].real() * outbuf[i].real();
        double im2 = outbuf[i].imag() * outbuf[i].imag();

        double result = 10 * log10(sqrt( re2 + im2) / pow(2,7) / N);
        //   double freq = (ND*(double)i/N*fs-fs/2.0)/1.e6;
        double fs = 2.0e6;
     //   data.fftVector.append((float)i/N * fs - fs/2.0)/1.e6, result);
    }

    return data;
}
