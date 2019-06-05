#ifndef LOGGER_H
#define LOGGER_H

#include <QMutexLocker>
#include <QStack>

#include "logger_global.h"
#include "ILogger.h"

class LOGGERSHARED_EXPORT Logger: public ILogger
{
    QStack<QString> _stack;
    QMutex _mutex;
    int _size = 1000;

public:
    Logger(int size);
    ~Logger() override;
    void push(const QString &text,
              TypeLog type = TypeLog::Default) override;
    QString pop() override;

    bool isEmpty() override;
};

#endif // LOGGER_H
