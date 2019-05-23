#ifndef IRECIVERDEVICE_H
#define IRECIVERDEVICE_H

#include <stdint.h>
#include <QVector>
#include <QSharedPointer>

class ILogger;

class IReciverDevice
{
public:
    virtual ~IReciverDevice(){}
    /*!
     * \brief открытие символьного устройства
     * \return true при удачной операции, иначе false
     */
    virtual bool openDevice() = 0;
    virtual bool isOpenDevice() = 0;
    virtual void closeDevice() = 0;

    virtual void setLogger(QSharedPointer<ILogger>) = 0;
    /*!
     * \brief перевод устроства в режим имитации,
     * если реально устроства нет
     * \return true - включен режим имитации,
     * false - работает реальное устроство
     */
    virtual bool isImitMode() = 0;
    /*!
     * \brief установка режима имитации
     * \param [in] state - включить или выключить имитацию
     *
     */
    virtual void setImitMode(bool state) = 0;

    virtual QVector<uint8_t> getDataBlock(size_t) = 0;
    virtual const uint8_t* getDataBlockPtr(size_t) = 0;
    virtual bool readDataBlock(QVector<uint8_t>&, size_t) = 0;
    virtual void setFreq(uint32_t freq) = 0;
protected:
    virtual bool initDevice() = 0;
};

#endif // IRECIVERDEVICE_H
