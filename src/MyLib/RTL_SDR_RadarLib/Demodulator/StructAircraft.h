#ifndef STRUCTAIRCRAFT_H
#define STRUCTAIRCRAFT_H

#include <stdint.h>

constexpr char SIZE_TEXT = 9;

/*!
 * @brief  Характеристики объекта.
 */
#pragma pack(push,1)
struct StructAircraft
{
    uint32_t icao;      /* ICAO address */
    char flight[SIZE_TEXT];     /* Flight number */
    uint32_t altitude;       /* Altitude */
    uint32_t speed;          /* Velocity computed from EW and NS components. */
    uint32_t course;          /* Angle of flight. */ 
    int32_t lat;
    int32_t lon;
    int64_t seen;        /* Time at which the last packet was received. */
    uint64_t messages;      /* Number of Mode S messages received. */
};
#pragma pack(pop)

#endif // STRUCTAIRCRAFT_H
