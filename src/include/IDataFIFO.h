#ifndef IDATAFIFO_H
#define IDATAFIFO_H

#include "dsp/SrcDataAdc.h"

class IDataFIFO
{
public:
    virtual ~IDataFIFO(){}

    virtual void pushData(SrcDataAdc data) = 0;
    virtual SrcDataAdc popData() = 0;
    virtual bool isFull() = 0;
    virtual bool isEmpty() = 0;
    virtual int depth() = 0;
};
#endif // IDATAFIFO_H
