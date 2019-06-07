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
    virtual void push(const QString& text,
                      TypeLog type = TypeLog::Default) = 0;
    virtual QString pop() = 0;

    virtual bool isEmpty() = 0;
    virtual int32_t size() = 0;
    virtual int32_t countMsg() = 0;
};

#endif // ILOGGER

