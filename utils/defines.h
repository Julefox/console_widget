#pragma once

#include <QWidget>

#if QT_VERSION_MAJOR == 6
# define QT_6
#elif QT_VERSION_MAJOR == 5
# define QT_5
#endif

#define U8( str ) QString( QStringLiteral( str ) )
