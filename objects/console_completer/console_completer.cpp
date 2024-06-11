#include "console_completer.h"

#include <QKeyEvent>
#include <QListView>

ConsoleCompleter::ConsoleCompleter( QObject* parent ) : QCompleter( parent )
{
	setFilterMode( Qt::MatchContains );
	setCompletionRole( Qt::UserRole );
	setCaseSensitivity( Qt::CaseInsensitive );
}

void ConsoleCompleter::SetFont( const QFont& font )
{
	auto* view = new QListView;
	view->setFont( font );

	setPopup( view );
}

bool ConsoleCompleter::eventFilter( QObject* obj, QEvent* event )
{
	if ( event->type() == QEvent::KeyPress )
	{
		if ( const auto keyEvent = dynamic_cast < QKeyEvent* >( event ); keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Space )
		{
			if ( popup()->isVisible() )
			{
				if ( QModelIndex currentIndex = popup()->currentIndex(); !currentIndex.isValid() )
				{
					currentIndex = popup()->model()->index( 0, 0 ); // Sélectionne la première suggestion
					popup()->setCurrentIndex( currentIndex );
				}
				emit TabPressed();
				return true; // Event handled
			}
		}
	}
	return QCompleter::eventFilter( obj, event );
}
