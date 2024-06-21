#pragma once

#include <QStandardItemModel>

#include "console_widget_global.h"
#include "utils/const.h"

#include "objects/line_data/line_data.h"
#include "objects/console_printer/console_printer.h"

#include "ui_console_widget.h"

#include "objects/console_completer/console_completer.h"

QT_BEGIN_NAMESPACE namespace Ui
{
	class ConsoleWidgetClass;
};

QT_END_NAMESPACE class ConVarBase;

class CONSOLE_WIDGET_EXPORT ConsoleWidget final : public QWidget
{
	Q_OBJECT public:
	explicit ConsoleWidget( QWidget* parent = nullptr );

	ConsolePrinter Print( const ePrintType type = ePrintType::PRINT_INFO ) { return ConsolePrinter( this, type ); }

	void AddLine( const QString& line, ePrintType type = ePrintType::PRINT_INFO );
	void Clear();

	void SetupFonts( const QFont& consoleFont, const QFont& commandFont, const QFont& completerFont ) const;
	void SetupConsoleFont( const QFont& font ) const { ui->consoleTextEdit->setFont( font ); }
	void SetupCommandFont( const QFont& font ) const { ui->commandLineEdit->setFont( font ); }
	void SetupCompleterFont( const QFont& font ) const { completer->SetFont( font ); }

	void UpdateCommands() const;

	static QList < ConsoleWidget* > GetConsoles() { return consoles; }

	static GlobalConsolePrinter PrintGlobal( const ePrintType type = ePrintType::PRINT_INFO ) { return GlobalConsolePrinter( type ); }

	static void SetPrintColor( const ePrintType type, const QColor& color ) { printColors[ type ] = color; }
	static QColor GetPrintColor( const ePrintType type ) { return printColors[ type ]; }

	static void SetupConsolesFonts( const QFont& font, const QFont& commandFont, const QFont& completerFont );

	static void UpdateConsolesCommands();

protected:
	void keyPressEvent( QKeyEvent* event ) override;

private slots:
	void OnCommandEntered();
	void SaveLogs();
	void TabPressed() const;
	void FilterChanged( const QString& filter ) const;

private:
	Ui::ConsoleWidgetClass* ui;
	ConsoleCompleter* completer = nullptr;
	QStandardItemModel* completerModel;

	QStringList commandBuffer;
	int bufferIndex = -1;

	QList < LineData > lines;

	void RemoveFirstLine() const;
	[[nodiscard]] bool FilterEnabled() const { return !ui->filterLineEdit->text().isEmpty(); }

	void UpdateConsoleColors() const;

	inline static QMap < ePrintType, QColor > printColors = {
		{ ePrintType::PRINT_INFO, QColor( "#4A90E2" ) }, // Blue light
		{ ePrintType::PRINT_NOTICE, QColor( "#87CEEB" ) }, // Blue sky
		{ ePrintType::PRINT_WARNING, QColor( "#F5A623" ) }, // Orange light
		{ ePrintType::PRINT_ERROR, QColor( "#D0021B" ) }, // Red light
		{ ePrintType::PRINT_SUCCESS, QColor( "#7ED321" ) } // Green light
	};

	inline static QList < ConsoleWidget* > consoles;

	inline static QColor disabledLineColor = { "#D3D3D3" }; // Light gray

	static constexpr int MaxCommandBuffer = 16;
	static constexpr int MaxLineCount = 1000;
};
