#ifndef DSPLIB_GLOBAL_H
#define DSPLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DSPLIB_LIBRARY)
#  define DSPLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DSPLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DSPLIB_GLOBAL_H
