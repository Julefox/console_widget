#include "console_widget.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QTextCharFormat>
#include <QAbstractItemView>
#include <QStandardItem>
#include <QStringListModel>

#include "objects/con_var/con_var.h"

ConsoleWidget::ConsoleWidget( QWidget* parent ) : QWidget( parent ), ui( new Ui::ConsoleWidgetClass() ), completer( new ConsoleCompleter( this ) ), completerModel( new QStandardItemModel( this ) )
{
	ui->setupUi( this );

	QFont font = ui->consoleTextEdit->font();
	font.setPointSize( 10 );
	font.setFamily( "Cascadia Mono" );

	QFont completerFont = font;
	completerFont.setBold( true );

	SetupFonts( font, font, completerFont );

	consoles.push_back( this );

	connect( ui->commandLineEdit, &QLineEdit::returnPressed, this, &ConsoleWidget::OnCommandEntered );
	//connect(ui->saveLogButton, &QPushButton::clicked, this, &ConsoleWidget::SaveLogs);
	connect( completer, &ConsoleCompleter::TabPressed, this, &ConsoleWidget::TabPressed );

	ui->commandLineEdit->setCompleter( completer );
	completer->setModel( completerModel );

	UpdateCommands();
}

void ConsoleWidget::AddLine( const QString& line, const ePrintType type )
{
	LineData data;
	data.Text = line;
	data.Type = type;
	data.Color = printColors[ type ];
	lines.push_back( data );

	if ( lines.size() > MaxLineCount )
	{
		RemoveFirstLine();
		lines.pop_front();
	}

	QTextCharFormat format;
	format.setForeground( printColors[ type ] );
	ui->consoleTextEdit->moveCursor( QTextCursor::End );
	ui->consoleTextEdit->setCurrentCharFormat( format );

	QString str;
	QString timestamp = QDateTime::currentDateTime().toString( "[yyyy-MM-dd hh:mm:ss]" );

	switch ( type )
	{
	case ePrintType::INFO:
		str = QString( "%1 [INFO]     %2" ).arg( timestamp, line );
		break;
	case ePrintType::NOTICE:
		str = QString( "%1 [NOTICE]   %2" ).arg( timestamp, line );
		break;
	case ePrintType::WARNING:
		str = QString( "%1 [WARNING]  %2" ).arg( timestamp, line );
		break;
	case ePrintType::ERROR:
		str = QString( "%1 [ERROR]    %2" ).arg( timestamp, line );
		break;
	case ePrintType::SUCCESS:
		str = QString( "%1 [SUCCESS]  %2" ).arg( timestamp, line );
		break;
	}

	ui->consoleTextEdit->appendPlainText( str );
}

void ConsoleWidget::Clear()
{
	ui->consoleTextEdit->clear();
	lines.clear();
}

void ConsoleWidget::SetupFonts( const QFont& consoleFont, const QFont& commandFont, const QFont& completerFont ) const
{
	SetupConsoleFont( consoleFont );
	SetupCommandFont( commandFont );
	SetupCompleterFont( completerFont );
}

void ConsoleWidget::UpdateCommands() const
{
	QStringList suggestions, commands;

	for ( const auto& [ name, var ] : ConVarManager::GetConVars() )
	{
		QString suggestion = name;
		QString command = name;

		if ( var->IsVariable() )
		{
			if ( const auto conVarFloat = ConVarManager::GetConVar < float >( name ) )
				suggestion.append( QString( " = [%1]" ).arg( QString::number( conVarFloat->GetValue() ) ) );
			else if ( const auto conVarInt = ConVarManager::GetConVar < int >( name ) )
				suggestion.append( QString( " = [%1]" ).arg( QString::number( conVarInt->GetValue() ) ) );
			else if ( const auto conVarBool = ConVarManager::GetConVar < bool >( name ) )
			{
				const QString boolValue = conVarBool->GetValue() == 1 ? "true" : "false";

				suggestion.append( QString( " = [%1]" ).arg( boolValue ) );
			}
			else if ( const auto conVarString = ConVarManager::GetConVar < QString >( name ) )
				suggestion.append( QString( " = [%1]" ).arg( conVarString->GetValue() ) );
			else { suggestion.append( " = <unknown type>" ); }

			command.append( " " );
		}

		if ( const QString help = var->GetDescription(); !help.isEmpty() )
			suggestion.append( " - " + help );

		suggestions.push_back( suggestion );
		commands.push_back( command );
	}

#if defined( QT_5 )
	const int size = commands.size();
#elif defined( QT_6 )
	const int size = static_cast < int >( commands.size() );
#endif

	completerModel->clear();

	for ( int i = 0; i < size; ++i )
	{
		const auto standardItem = new QStandardItem( suggestions[ i ] );
		standardItem->setData( commands[ i ], Qt::UserRole );
		completerModel->setItem( i, 0, standardItem );
	}
}

void ConsoleWidget::SetupConsolesFonts( const QFont& font, const QFont& commandFont, const QFont& completerFont )
{
	for ( const ConsoleWidget* console : consoles )
	{
		if ( console )
			console->SetupFonts( font, commandFont, completerFont );
	}
}

void ConsoleWidget::UpdateConsolesCommands()
{
	for ( const ConsoleWidget* console : consoles )
	{
		if ( console )
			console->UpdateCommands();
	}
}

void ConsoleWidget::keyPressEvent( QKeyEvent* event )
{
#if defined( QT_6 )
	const int bufferSize = static_cast < int >( commandBuffer.size() );
#elif defined( QT_5 )
	const int bufferSize = commandBuffer.size();
#endif

	if ( event->key() == Qt::Key_Up )
	{
		if ( bufferIndex < bufferSize - 1 )
		{
			++bufferIndex;
			ui->commandLineEdit->setText( commandBuffer[ bufferIndex ] );
		}
	}
	else if ( event->key() == Qt::Key_Down )
	{
		if ( bufferIndex > 0 )
		{
			--bufferIndex;
			ui->commandLineEdit->setText( commandBuffer[ bufferIndex ] );
		}
		else
		{
			ui->commandLineEdit->clear();
			bufferIndex = -1;
		}
	}
	else { QWidget::keyPressEvent( event ); }
}

void ConsoleWidget::OnCommandEntered()
{
	const QString& command = ui->commandLineEdit->text();

	if ( command.isEmpty() )
		return;

	if ( commandBuffer.empty() || commandBuffer.back() != command )
		commandBuffer.push_front( command );

	if ( commandBuffer.size() > MaxCommandBuffer )
		commandBuffer.pop_back();

	const QStringList args = command.split( ' ', Qt::SkipEmptyParts );

	// command is the first argument ( args[ 0 ] )
	if ( ConVarBase* conVar = ConVarManager::GetConVar( args[ 0 ] ) )
	{
		const int argCount = conVar->GetArgumentCount();

		if ( args.size() < argCount + 1 )
			ConVarManager::PrintInvalidArgument( this, conVar, args[ 0 ] );
		else
			conVar->Callback( conVar, args, this );
	}
	else
	{
		Print( ePrintType::ERROR ) << tr( "Unknown command: %1" ).arg( args[ 0 ] );
		Print( ePrintType::INFO ) << tr( "Type 'help' for a list of available commands" );
	}

	bufferIndex = -1;
	ui->commandLineEdit->clear();
}

void ConsoleWidget::SaveLogs()
{
	const QDateTime currentDateTime = QDateTime::currentDateTime();

	const QString defaultFileName = QString( "log_%1" ).arg( currentDateTime.toString( "dd-MM-yy_hh-mm" ) );

	const QString fileName = QFileDialog::getSaveFileName( this, tr( "Sauvegarder les logs" ), QDir::currentPath() + "/" + defaultFileName + ".txt", "Text files (*.txt)" );

	if ( fileName.isEmpty() )
		return;

	QFile file( fileName );

	if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
	{
		QTextStream out( &file );
#if defined( QT_6 )
		out.setEncoding( QStringConverter::Utf8 );
#elif defined( QT_5 )
		out.setCodec( "UTF-8" );
#endif
		out << ui->consoleTextEdit->toPlainText();
	}
}

void ConsoleWidget::TabPressed() const
{
	completer->popup()->hide();
	ui->commandLineEdit->setFocus();
}

void ConsoleWidget::RemoveFirstLine() const
{
	QTextCursor cursor( ui->consoleTextEdit->document() );

	cursor.movePosition( QTextCursor::Start );
	cursor.select( QTextCursor::LineUnderCursor );
	cursor.removeSelectedText();
	cursor.deleteChar();
}
