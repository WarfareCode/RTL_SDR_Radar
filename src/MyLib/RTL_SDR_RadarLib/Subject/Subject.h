#ifndef SUBJECT_H
#define SUBJECT_H

#include "subject_global.h"

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedPointer>

#include "IObserver.h"

class SUBJECTSHARED_EXPORT Subject
{
    QMutex _mutex;
public:
    virtual ~Subject();

    void Attach(IObserver* o);
    void Deatach(IObserver* o);
    void Notify();
protected:
    Subject();

private:

    QSharedPointer<QVector<IObserver*>> _observers;
};

#endif // SUBJECT_H
