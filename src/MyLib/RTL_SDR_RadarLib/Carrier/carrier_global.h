#ifndef CARRIER_GLOBAL_H
#define CARRIER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CARRIER_LIBRARY)
#  define CARRIERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CARRIERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
