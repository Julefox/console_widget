#pragma once

#include <functional>

#include "../../console_widget.h"
#include "../../utils/const.h"

class ConVarBase;

enum class eCVarType
{
	STRING,
	INT,
	FLOAT,
	BOOL
};

class ConVarBase
{
public:
	explicit ConVarBase( const QString& name );
	virtual ~ConVarBase() = default;

	[[nodiscard]] QString GetName() const { return name; }
	[[nodiscard]] QString GetDescription() const { return description; }
	[[nodiscard]] bool IsVariable() const { return isVariable; }
	[[nodiscard]] bool HasArguments() const { return !arguments.isEmpty(); }
	[[nodiscard]] QStringList GetArguments() const { return arguments; }
#if defined( QT_6 )
	[[nodiscard]] int GetArgumentCount() const { return static_cast < int >( arguments.size() ); }
#elif defined( QT_5 )
	[[nodiscard]] int GetArgumentCount() const { return arguments.size(); }
#endif
	bool Callback( ConVarBase* var, const QStringList& args, ConsoleWidget* console ) const { return callback( var, args, console ); }

	void SetName( const QString& nameStr ) { name = nameStr; }
	void SetDescription( const QString& helpStr ) { description = helpStr; }
	void SetIsVariable( const bool value ) { isVariable = value; }
	void SetArguments( const QStringList& args ) { arguments = args; }
	void SetCallback( const ConVarCallback& func ) { callback = func; }

protected:
	QString name;
	QString description;
	bool isVariable = false;
	QStringList arguments;

	ConVarCallback callback;
};

template < typename T >
class ConVar final : public ConVarBase
{
public:
	static_assert( std::is_same_v < T, QString > || std::is_same_v < T, int > || std::is_same_v < T, float > || std::is_same_v < T, bool >, "ConVar can only be instantiated with QString, int, float, or bool" );

	ConVar( const QString& cvarName, T value ) : ConVarBase( cvarName ), value( value ), defaultValue( value ) {}

	[[nodiscard]] bool HasMinValue() const { return hasMinValue; }
	[[nodiscard]] T GetMinValue() const { return minValue; }
	[[nodiscard]] bool HasMaxValue() const { return hasMaxValue; }
	[[nodiscard]] T GetMaxValue() const { return maxValue; }

	void SetValue( const T& newValue, ConsoleWidget* console )
	{
		if ( newValue == value )
			return;

		if ( console )
		{
			if ( HasMinValue() && newValue < GetMinValue() || HasMaxValue() && newValue > GetMaxValue() )
			{
				console->Print( ePrintType::PRINT_ERROR ) << U8( "ConVar::SetValue() Error: Value is out of range, expected between %1 - %2" ).arg( QString::number( GetMinValue() ) ).arg( QString::number( GetMaxValue() ) );
				return;
			}

			if ( this->IsVariable() && console )
			{
				const auto message = ConVarChangeMessage.arg( this->GetName() );
				if ( std::is_same_v < T, QString > ) { console->Print( ePrintType::PRINT_NOTICE ) << QString( message ).arg( this->GetValue() ).arg( newValue ); }
				else if ( std::is_same_v < T, int > || std::is_same_v < T, float > ) { console->Print( ePrintType::PRINT_NOTICE ) << QString( message ).arg( QString::number( this->GetValue() ) ).arg( QString::number( newValue ) ); }
				else if ( std::is_same_v < T, bool > )
				{
					const QString boolValue = this->GetValue() == true ? "true" : "false";
					const QString newBoolValue = newValue == true ? "true" : "false";

					console->Print( ePrintType::PRINT_NOTICE ) << QString( message ).arg( boolValue ).arg( newBoolValue );
				}
			}
		}

		value = newValue;
		//ConsoleWidget::UpdateCommands();
	}

	void SetMinValue( const T minVal )
	{
		minValue = minVal;
		hasMinValue = true;
	}

	void SetMaxValue( const T maxVal )
	{
		maxValue = maxVal;
		hasMaxValue = true;
	}

	[[nodiscard]] T GetValue() const { return value; }
	[[nodiscard]] T GetDefaultValue() const { return defaultValue; }

	static eCVarType GetType( const ConVarBase* var );

private:
	T value;
	T defaultValue;

	bool hasMinValue = false;
	T minValue;
	bool hasMaxValue = false;
	T maxValue;

	inline static const auto ConVarChangeMessage = U8( "ConVar [%1] changed: %2 => %3" );
};

class ConVarManager final
{
public:
	static void ConVarInit();
	static void RegisterConVar( ConVarBase* var );
	static void RegisterAlias( const QString& alias, const QString& originalName );

	static ConVar < float >* RegisterFloatConVar( const QString& name, float defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args = {}, bool isVariable = false );
	static ConVar < float >* RegisterFloatConVar( const QString& name, float defaultValue, const QString& description, const ConVarCallback& callback, bool isVariable );
	static ConVar < int >* RegisterIntConVar( const QString& name, int defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args = {}, bool isVariable = false );
	static ConVar < int >* RegisterIntConVar( const QString& name, int defaultValue, const QString& description, const ConVarCallback& callback, bool isVariable );
	static ConVar < bool >* RegisterBoolConVar( const QString& name, bool defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args = {}, bool isVariable = false );
	static ConVar < bool >* RegisterBoolConVar( const QString& name, bool defaultValue, const QString& description, const ConVarCallback& callback, bool isVariable );
	static ConVar < QString >* RegisterStringConVar( const QString& name, const QString& defaultValue, const QString& description, const ConVarCallback& callback, const QStringList& args = {}, bool isVariable = false );
	static ConVar < QString >* RegisterStringConVar( const QString& name, const QString& defaultValue, const QString& description, const ConVarCallback& callback, bool isVariable );

	static void UnregisterConVar( const QString& name );

	static bool PrintInvalidArgument( ConsoleWidget* console, const ConVarBase* var, const QString& conVarName );

	static const auto& GetConVars() { return conVars; }

	[[nodiscard]] static ConVarBase* GetConVar( const QString& name );

	template < typename T >
	[[nodiscard]] static ConVar < T >* GetConVar( const QString& name )
	{
		if ( ConVarBase* var = GetConVar( name ); var )
			return dynamic_cast < ConVar < T >* >( var );

		return nullptr;
	}

	template < typename T >
	static void SetConVarValue( const QString& name, T newValue, ConsoleWidget* console = nullptr )
	{
		if ( ConVar < T >* var = GetConVar < T >( name ); var )
			var->SetValue( newValue, console );
	}

	[[nodiscard]] static bool GetConVarValueBool( const QString& name );
	[[nodiscard]] static int GetConVarValueInt( const QString& name );
	[[nodiscard]] static float GetConVarValueFloat( const QString& name );
	[[nodiscard]] static QString GetConVarValueString( const QString& name );

private:
	inline static std::map < QString, ConVarBase* > conVars;

	static bool ClearConsoleCallback( ConVarBase*, const QStringList&, ConsoleWidget* );
	static bool HelpCallback( ConVarBase*, const QStringList&, ConsoleWidget* );
	static bool PrintCallback( ConVarBase*, const QStringList&, ConsoleWidget* );
};
