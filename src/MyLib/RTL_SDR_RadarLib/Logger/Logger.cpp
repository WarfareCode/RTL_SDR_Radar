#include "Logger.h"


Logger::Logger()
{
}

void Logger::add(QString text, TypeLog type)
{
    Q_UNUSED(type);
    qDebug()<<text;
}
