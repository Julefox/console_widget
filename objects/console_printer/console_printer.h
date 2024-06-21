#pragma once

#include <sstream>

#include <QString>

#include "utils/const.h"

class ConsoleWidget;

class ConsolePrinter
{
public:
	explicit ConsolePrinter( const ePrintType printType = ePrintType::PRINT_INFO ) : type( printType ) {}
	explicit ConsolePrinter( ConsoleWidget* consolePtr, const ePrintType printType = ePrintType::PRINT_INFO ) : type( printType ), console( consolePtr ) {}
	virtual ~ConsolePrinter();

	template < typename T >
	ConsolePrinter& operator<<( const T& value )
	{
		stream << value << ' ';
		return *this;
	}

	ConsolePrinter& operator<<( const QString& value )
	{
		stream << value.toStdString() << ' ';
		return *this;
	}

	ConsolePrinter( const ConsolePrinter& ) = delete;
	ConsolePrinter& operator=( const ConsolePrinter& ) = delete;

protected:
	ePrintType type;
	ConsoleWidget* console = nullptr;
	std::ostringstream stream;
};

class GlobalConsolePrinter final : public ConsolePrinter
{
public:
	explicit GlobalConsolePrinter( const ePrintType printType = ePrintType::PRINT_INFO ) : ConsolePrinter( printType ) {}
	~GlobalConsolePrinter() override;
};
