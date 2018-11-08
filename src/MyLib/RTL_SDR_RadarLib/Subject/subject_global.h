#ifndef SUBJECT_GLOBAL_H
#define SUBJECT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SUBJECT_LIBRARY)
#  define SUBJECTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SUBJECTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SUBJECT_GLOBAL_H
