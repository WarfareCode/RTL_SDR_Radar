#ifndef MAPLIB_H
#define MAPLIB_H

#include "maplib_global.h"
#include "IMapController.h"
#include "ICarrierClass.h"
#include <QSharedPointer>

enum class TypeMap
{
    MapServer = 0,
    OSM,
    NullMap
};

class MAPLIBSHARED_EXPORT MapLib
{
    IMapController* _map;
public:
    MapLib(TypeMap map, QSharedPointer<ICarrierClass>);
    ~MapLib();
    IMapController *getMapController();

};

#endif // MAPLIB_H
