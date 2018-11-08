#include "NullCarrier.h"


NullCarrier::NullCarrier()
{

    _ptrNavigation = QSharedPointer<INavigationSystem>(new NullNavigationSystem());
    qDebug()<<"NullCarrier create";
}

NullCarrier::~NullCarrier()
{
    qDebug()<<"NullCarrier delete";
    _ptrNavigation.clear();
}
