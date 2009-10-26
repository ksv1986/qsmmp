#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QDebug>
#include <QUrl>

#include "mytableview.h"
#include "mytreeview.h"
#include "abstractplaylistmodel.h"

MyTableView::MyTableView(QWidget *parent=0)
	: QTableView(parent)
{
}

void MyTableView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
	event->setDropAction(Qt::CopyAction);
	event->accept();
    }
}

void MyTableView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
	event->setDropAction(Qt::CopyAction);
	event->accept();
    }
}

void MyTableView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
	AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());
	foreach(QUrl url, event->mimeData()->urls())
	{
	    playlist->addItem(url.toLocalFile());
	}
	event->setDropAction(Qt::CopyAction);
	event->accept();
    }
}
