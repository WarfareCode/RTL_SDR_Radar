#include "MapLib.h"

MapLib::MapLib(TypeMap map,
               QSharedPointer<ICarrierClass>)
{
    Q_UNUSED(map);
}


MapLib::~MapLib()
{

}

IMapController *MapLib::getMapController()
{
     return nullptr;
}
