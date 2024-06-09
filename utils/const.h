#pragma once

#include <QStringList>

enum class ePrintType : int
{
	INFO,
	NOTICE,
	WARNING,
	SUCCESS,
	ERROR
};

class ConVarBase;
class ConsoleWidget;

using ConVarCallback = std::function < bool( ConVarBase*, const QStringList&, ConsoleWidget* ) >;
