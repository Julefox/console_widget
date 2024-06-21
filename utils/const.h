#pragma once

#include <QStringList>

enum class ePrintType : int
{
	PRINT_INFO,
	PRINT_NOTICE,
	PRINT_WARNING,
	PRINT_SUCCESS,
	PRINT_ERROR
};

class ConVarBase;
class ConsoleWidget;

using ConVarCallback = std::function < bool( ConVarBase*, const QStringList&, ConsoleWidget* ) >;
