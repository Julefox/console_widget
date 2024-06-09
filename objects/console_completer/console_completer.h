#pragma once

#include <QCompleter>

class ConsoleCompleter final : public QCompleter
{
	Q_OBJECT public:
	explicit ConsoleCompleter( QObject* parent );

	void SetFont( const QFont& font );

signals:
	void TabPressed();

protected:
	bool eventFilter( QObject* obj, QEvent* event ) override;
	[[nodiscard]] QStringList splitPath( const QString& path ) const override { return QStringList( path ); }
	[[nodiscard]] QString pathFromIndex( const QModelIndex& index ) const override { return index.data( Qt::UserRole ).toString(); }
};
