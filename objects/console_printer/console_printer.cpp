#include "console_printer.h"
#include "console_widget.h"

ConsolePrinter::~ConsolePrinter()
{
	if ( console )
		console->Print( type ) << QString::fromStdString( stream.str() );
}

GlobalConsolePrinter::~GlobalConsolePrinter()
{
	for ( ConsoleWidget* console : ConsoleWidget::GetConsoles() )
	{
		if ( console )
			console->Print( type ) << QString::fromStdString( stream.str() );
	}
}
