#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(RENDERMODULE_LIB)
#  define RENDERMODULE_EXPORT Q_DECL_EXPORT
# else
#  define RENDERMODULE_EXPORT Q_DECL_IMPORT
# endif
#else
# define RENDERMODULE_EXPORT
#endif
