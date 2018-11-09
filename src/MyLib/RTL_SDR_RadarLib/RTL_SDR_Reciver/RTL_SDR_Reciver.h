#ifndef RTL_SDR_RECIVER_H
#define RTL_SDR_RECIVER_H

#include "rtl_sdr_reciver_global.h"

#include <QString>
#include <QHash>
#include <QSharedPointer>
#include <QVector>
#include <QDebug>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include "../../../../import/sdr_dev/include/rtl-sdr.h"
#include "../../../../import/sdr_dev/include/constant.h"

#include "../../../include/IReciverDevice.h"

//namespace RTL_SDR
//{
//    void rtlsdrCallback(unsigned char *buf, uint32_t len, void *ctx) ;
//    void* readerThreadEntryPoint(void *arg);

class RTL_SDR_RECIVERSHARED_EXPORT RTL_SDR_Reciver : public IReciverDevice
{

    bool _isOpen = false;
    char vendor[256], product[256], serial[256];

    int _dev_index = 0;
    int _gain = MODES_MAX_GAIN;
    int _enable_agc = 0;
    int _freq = MODES_DEFAULT_FREQ;

    QVector<uint8_t> _data;            /* Raw IQ samples buffer */
    rtlsdr_dev_t *_dev;

public:

    RTL_SDR_Reciver();
    ~RTL_SDR_Reciver() override;

    bool openDevice() override;
    bool isOpenDevice() override;
    void closeDevice() override;

    QVector<uint8_t> getDataBlock(size_t size) override;
    const uint8_t *getDataBlockPtr(size_t size) override;

protected:
    bool initDevice() override;
};

//}

#endif // RTL_SDR_RECIVER_H
