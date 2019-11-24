#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <memory>

#include <QtCore>
#include <QtDebug>
#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>

#if defined(Q_OS_WIN32)
#include <QtWinExtras>
#endif

#ifdef Q_OS_MACOS
#include <QtMacExtras>
#endif

#ifdef _MSC_VER
#pragma warning pop
#endif
