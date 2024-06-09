#include "console_printer.h"
#include "console_widget.h"

ConsolePrinter::~ConsolePrinter()
{
	if ( console )
		console->AddLine( QString::fromStdString( stream.str() ), type );
}

GlobalConsolePrinter::~GlobalConsolePrinter()
{
	for ( ConsoleWidget* console : ConsoleWidget::GetConsoles() )
	{
		if ( console )
			console->AddLine( QString::fromStdString( stream.str() ), type );
	}
}
