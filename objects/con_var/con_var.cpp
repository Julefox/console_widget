#include "con_var.h"

ConVarBase::ConVarBase( const QString& name ) { this->name = name; }

template < typename T >
eCVarType ConVar < T >::GetType( const ConVarBase* var )
{
	const QString& name = var->GetName();

	if ( ConVarManager::GetConVar < QString >( name ) )
		return eCVarType::STRING;
	if ( ConVarManager::GetConVar < int >( name ) )
		return eCVarType::INT;
	if ( ConVarManager::GetConVar < float >( name ) )
		return eCVarType::FLOAT;
	if ( ConVarManager::GetConVar < bool >( name ) )
		return eCVarType::BOOL;

	return eCVarType::STRING;
}

void ConVarManager::ConVarInit()
{
	RegisterBoolConVar( "clear", false, "Clear the console", &ConVarManager::ClearConsoleCallback );
	RegisterAlias( "cls", "clear" );

	RegisterBoolConVar( "help", false, "Gives all available commands", &ConVarManager::HelpCallback );

	RegisterBoolConVar( "print", false, "Print a message in this console", &ConVarManager::PrintCallback, QStringList( "message_string" ) );
	RegisterAlias( "say", "print" );
}

void ConVarManager::RegisterConVar( ConVarBase* var )
{
	const QString name = var->GetName();
	if ( GetConVars().contains( name ) )
	{
		ConsoleWidget::PrintGlobal( ePrintType::ERROR ) << "ConVarManager::RegisterConVar() ConVar" << name << "already exists!";
		return;
	}

	conVars[ name ] = var;
}

void ConVarManager::RegisterAlias( const QString& alias, const QString& originalName )
{
	if ( auto* conVar = GetConVar( originalName ); conVar && !conVars.contains( alias ) )
		conVars.insert( { alias, conVar } );

	ConsoleWidget::PrintGlobal( ePrintType::ERROR ) << "ConVarManager::RegisterAlias() Alias" << alias << "for" << originalName << "already exists!";
}

ConVar < float >* ConVarManager::RegisterFloatConVar( const QString& name, const float defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args, const bool isVariable )
{
	ConVar < float >* var = new ConVar( name, defaultValue );
	var->SetDescription( description );
	var->SetCallback( callback );
	var->SetIsVariable( isVariable );
	var->SetArguments( args );
	RegisterConVar( var );

	return var;
}

ConVar < float >* ConVarManager::RegisterFloatConVar( const QString& name, const float defaultValue, const QString& description, const ConVarCallback& callback, const bool isVariable )
{
	return RegisterFloatConVar( name, defaultValue, description, callback, {}, isVariable );
}

ConVar < int >* ConVarManager::RegisterIntConVar( const QString& name, const int defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args, const bool isVariable )
{
	ConVar < int >* var = new ConVar( name, defaultValue );
	var->SetDescription( description );
	var->SetCallback( callback );
	var->SetIsVariable( isVariable );
	var->SetArguments( args );
	RegisterConVar( var );

	return var;
}

ConVar < int >* ConVarManager::RegisterIntConVar( const QString& name, const int defaultValue, const QString& description, const ConVarCallback& callback, const bool isVariable )
{
	return RegisterIntConVar( name, defaultValue, description, callback, {}, isVariable );
}

ConVar < bool >* ConVarManager::RegisterBoolConVar( const QString& name, const bool defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args, const bool isVariable )
{
	ConVar < bool >* var = new ConVar( name, defaultValue );
	var->SetDescription( description );
	var->SetCallback( callback );
	var->SetIsVariable( isVariable );
	var->SetArguments( args );
	RegisterConVar( var );

	return var;
}

ConVar < bool >* ConVarManager::RegisterBoolConVar( const QString& name, const bool defaultValue, const QString& description, const ConVarCallback& callback, const bool isVariable )
{
	return RegisterBoolConVar( name, defaultValue, description, callback, {}, isVariable );
}

ConVar < QString >* ConVarManager::RegisterStringConVar( const QString& name, const QString& defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args, const bool isVariable )
{
	ConVar < QString >* var = new ConVar( name, defaultValue );
	var->SetDescription( description );
	var->SetCallback( callback );
	var->SetIsVariable( isVariable );
	var->SetArguments( args );
	RegisterConVar( var );

	return var;
}

ConVar < QString >* ConVarManager::RegisterStringConVar( const QString& name, const QString& defaultValue, const QString& description, const ConVarCallback& callback, const bool isVariable )
{
	return RegisterStringConVar( name, defaultValue, description, callback, {}, isVariable );
}

void ConVarManager::UnregisterConVar( const QString& name )
{
	if ( const auto* conVar = GetConVar( name ); conVar )
		conVars.erase( name );
}

bool ConVarManager::PrintInvalidArgument( ConsoleWidget* console, const ConVarBase* var, const QString& conVarName )
{
	if ( !console )
		return false;

	QString message = QString( "Invalid argument for %1 usage:" ).arg( conVarName );

	for ( const QString& arg : var->GetArguments() )
		message += " <" + arg + ">";

	console->Print() << message;

	return false;
}

ConVarBase* ConVarManager::GetConVar( const QString& name )
{
	if ( conVars.contains( name ) )
		return conVars.at( name );

	return nullptr;
}

bool ConVarManager::GetConVarValueBool( const QString& name )
{
	if ( const auto* conVar = GetConVar < bool >( name ); conVar )
		return conVar->GetValue();

	return false;
}

int ConVarManager::GetConVarValueInt( const QString& name )
{
	if ( const auto* conVar = GetConVar < int >( name ); conVar )
		return conVar->GetValue();

	return 0;
}

float ConVarManager::GetConVarValueFloat( const QString& name )
{
	if ( const auto* conVar = GetConVar < bool >( name ); conVar )
		return conVar->GetValue();

	return 0.0f;
}

QString ConVarManager::GetConVarValueString( const QString& name )
{
	if ( const auto* conVar = GetConVar < QString >( name ); conVar )
		return conVar->GetValue();

	return "";
}

// Callbacks
bool ConVarManager::ClearConsoleCallback( ConVarBase*, const QStringList&, ConsoleWidget* console )
{
	if ( console )
		console->Clear();

	return console;
}

bool ConVarManager::HelpCallback( ConVarBase*, const QStringList&, ConsoleWidget* console )
{
	console->Print() << "--------------------COMMANDS--------------------";
	for ( const auto& [ key, value ] : GetConVars() )
	{
		QString message = key;

		if ( const QString description = value->GetDescription(); !description.isEmpty() )
			message += " - " + description;

		console->Print() << message;
	}
	console->Print() << "------------------------------------------------";

	return console;
}

bool ConVarManager::PrintCallback( ConVarBase*, const QStringList& args, ConsoleWidget* console )
{
	const QStringList sublist = args.mid( 1 );

	if ( console )
		console->Print() << sublist.join( " " );

	return console;
}
