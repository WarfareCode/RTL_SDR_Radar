#ifndef IRECIVERDEVICE_H
#define IRECIVERDEVICE_H

#include <stdint.h>
#include <QVector>

class IReciverDevice
{
public:
    virtual ~IReciverDevice(){}
    virtual bool openDevice() = 0;
    virtual bool isOpenDevice() = 0;
    virtual void closeDevice() = 0;

    virtual QVector<uint8_t> getDataBlock(size_t) = 0;
    virtual const uint8_t* getDataBlockPtr(size_t) = 0;
    virtual bool readDataBlock(QVector<uint8_t>&, size_t) = 0;
    virtual void setFreq(uint32_t freq) = 0;
protected:
    virtual bool initDevice() = 0;
};

#endif // IRECIVERDEVICE_H
