#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

#include "logger_global.h"

#include "ILogger.h"

class LOGGERSHARED_EXPORT Logger: public ILogger
{

public:
    Logger();
    void add(QString text, TypeLog type = TypeLog::Default);
};

#endif // LOGGER_H
