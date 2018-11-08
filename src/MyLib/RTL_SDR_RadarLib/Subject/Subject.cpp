#include "Subject.h"

Subject::Subject()
{
    _observers  = QSharedPointer<QVector<IObserver*>> (new QVector<IObserver*>);
}


Subject::~Subject()
{
    qDebug()<<"~Subject" << _observers->size();
    _observers.clear();
}

void Subject::Attach(IObserver* o)
{

    QMutexLocker _lock(&_mutex);
    if(o == nullptr)
    {
        qDebug()<<"Subject::Attach() -> IObserver nullptr ";
        return;
    }

    for(auto & iter: *_observers)
    {
        if(iter->getObserverId() == o->getObserverId())
        {
            qDebug()<<"this subscribe in vector";
            return;
        }
    }
    //Add new subscriber to new subscriber list
    _observers->push_back(o);
    qDebug()<<"sibscribe to pool"<< _observers->size();
}

void Subject::Deatach(IObserver* o)
{
    //Enter to locked section
    QMutexLocker _lock(&_mutex);
    if(_observers.isNull() || _observers->isEmpty())
    {
        qDebug()<<"no subscribers";
        return;
    }

    _observers->remove(_observers->indexOf(o));
    qDebug()<<"remove this subscribe in vector" << _observers->size();
}

void Subject::Notify()
{
    QMutexLocker _lock(&_mutex);
    if(_observers.isNull() || _observers->isEmpty())
    {
        qDebug()<<"Subject::Notify() -> No subscribers";
        return;
    }

    for(auto & iter: *_observers)
    {
        iter->update(this);
    }
}
