#ifndef ILOGGER
#define ILOGGER

#include <QString>

enum class TypeLog
{
    Default,
    Success,
    Warning,
    Error
};

class ILogger
{

public:
    virtual ~ILogger(){}
    virtual void add(QString text, TypeLog type = TypeLog::Default) = 0;
};

#endif // ILOGGER

