#pragma once

#include <QtCore/QtGlobal>
#include "utils/defines.h"

#ifndef BUILD_STATIC
# if defined(CONSOLE_WIDGET_LIB)
#  define CONSOLE_WIDGET_EXPORT Q_DECL_EXPORT
# else
#  define CONSOLE_WIDGET_EXPORT Q_DECL_IMPORT
# endif
#else
# define CONSOLE_WIDGET_EXPORT
#endif
