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

void MyTableView::selectAll()
{
    QTableView::selectAll();

    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());
    for(int i=0; i<playlist->count(); ++i)
    {
	playlist->setSelected(i, true);
    }
}

void MyTableView::mousePressEvent(QMouseEvent *e)
{
    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());

    int y = e->y();
    int row = rowAt(y);

    if (playlist->count() > row)
    {
	if (!(Qt::ControlModifier & e->modifiers () ||
		Qt::ShiftModifier & e->modifiers () ||
		playlist->isSelected(row)))
	    playlist->clearSelection();

	if (playlist->isSelected(row) && (e->modifiers() == Qt::NoModifier))
	    m_select_on_release = TRUE;

	m_pressed_row = row;
	if ((Qt::ShiftModifier & e->modifiers()))
	{

	    if (m_pressed_row > m_anchor_row)
	    {
		playlist->clearSelection();
		for (int j = m_anchor_row;j <= m_pressed_row;j++)
		{
		    playlist->setSelected(j, true);
		}
	      }
	      else
	      {
		  playlist->clearSelection();
		  for (int j = m_anchor_row;j >= m_pressed_row;j--)
		  {
		      playlist->setSelected(j, true);
		  }
	      }
	  }
	else
	 {
	     if (!playlist->isSelected(row) || (Qt::ControlModifier & e->modifiers()))
		 playlist->setSelected(row, !playlist->isSelected(row));
	 }

	 if (playlist->getSelection(m_pressed_row).count() == 1)
	     m_anchor_row = m_pressed_row;

	 update();
     }
     QTableView::mousePressEvent(e);
 }

void MyTableView::mouseReleaseEvent(QMouseEvent *e)
{
    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());

    int y = e->y();
    int row = rowAt(y);
    if ((e->modifiers() == Qt::NoModifier) && playlist->count() > row)
    {
	m_pressed_row = row;

	if (m_pressed_row > m_anchor_row)
	{
	    for (int j = m_anchor_row;j <= m_pressed_row;j++)
	    {
		playlist->setSelected(j, true);
	    }
	}
	else
	{
	    for (int j = m_anchor_row;j >= m_pressed_row;j--)
	    {
		playlist->setSelected(j, true);
	    }
	}

	if (playlist->getSelection(m_pressed_row).count() == 1)
	     m_anchor_row = m_pressed_row;

	update();
    }
    QTableView::mouseReleaseEvent(e);
}
