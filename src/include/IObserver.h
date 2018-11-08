#ifndef IOBSERVER
#define IOBSERVER
#include <sys/types.h>

class Subject;

class  IObserver
{
public:
    virtual ~IObserver(){}
    virtual void update(Subject* sub) = 0;
    virtual void unsubscribe() = 0;
    u_int64_t getObserverId() {return (u_int64_t)this;}
protected:
    IObserver(){}
};

#endif // IOBSERVER

