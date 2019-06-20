/*!
    \file
    \brief Интерфейсный класс для работы с демодулятором
    Наследуется от QRunnable, может быть запущен в отдельном потоке

    \author Данильченко Артем
    \version 0.1
    \date декабрь 2018 г.
*/
#ifndef IDEMODULATOR_H
#define IDEMODULATOR_H

#include <QVector>
#include <QRunnable>
#include <QByteArray>
#include <QSharedPointer>

class IPoolObject;
class ILogger;

class IDemodulator :  public QRunnable
{
public:
    virtual ~IDemodulator(){}
    /*!
     *  \brief внедрение зависимости модуля логгирования
     */
    virtual void setLogger(QSharedPointer<ILogger> log) = 0;
    /*!
     *  \brief копирование массива данных для выполнения демодуляции
     */
    virtual bool setDataForDemodulate(const QVector<uint8_t>& vector) = 0;
    /*!
     *  \brief Получение сериализованных данных об обнаруженных самолётах
     *         в виде байтового массива для передачи по сети
     *  \return байтовый массив сериализованных объектов
     */
    virtual QByteArray getRawDump() = 0;
    /*!
     *  \brief Функция демодуляции, на случай использования класса без QThreadPool
     *  \return результат выполнения
     */
    virtual bool demodulate() = 0;

    /*!
     *  \brief Функция получения количества обнаруженных объектов
     *  \return количество объектов или -1 в случае ошибки
     */
    virtual int32_t getCountObject() = 0;
};

#endif // IDEMODULATOR_H
