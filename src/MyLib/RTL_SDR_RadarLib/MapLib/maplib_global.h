#ifndef MAPLIB_GLOBAL_H
#define MAPLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MAPLIB_LIBRARY)
#  define MAPLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MAPLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MAPLIB_GLOBAL_H
