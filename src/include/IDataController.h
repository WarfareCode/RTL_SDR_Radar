#ifndef IDATACONTROLLER_H
#define IDATACONTROLLER_H

#include <stdint.h>

class IDataController
{
public:
    virtual ~IDataController(){}

    virtual uint8_t* getDataToChart() = 0;

    virtual void run() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() = 0;
};
#endif // IDATACONTROLLER_H
