#include <QDebug>
#include "Logger.h"

Logger::Logger(int size) : _size(size)
{
}

Logger::~Logger()
{

}

void Logger::push(const QString &text, TypeLog type)
{
    QMutexLocker locker(&_mutex);
    if(_stack.size() < _size)
        _stack.push(text);
}

QString Logger::pop()
{
    QMutexLocker locker(&_mutex);
    if(!_stack.isEmpty())
        return _stack.pop();
    else
        return QString();
}

bool Logger::isEmpty()
{
    QMutexLocker locker(&_mutex);
    return _stack.isEmpty();
}


