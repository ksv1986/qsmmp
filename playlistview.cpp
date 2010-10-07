#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QDebug>
#include <QUrl>
#include <QHeaderView>
#include <QAction>
#include <QMap>
#include <QApplication>

#include "playlistview.h"
#include "mytreeview.h"
#include "abstractplaylistmodel.h"
#include "settings.h"

PlaylistView::PlaylistView(QWidget *parent=0)
        : QTreeView(parent)
{
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setAlternatingRowColors(true);
    header()->setMovable(true);
    header()->setClickable(true);
    connect(header(), SIGNAL(sectionClicked(int)), this, SLOT(sectionClicked(int)));
    setAutoScroll(true);
}

PlaylistView::~PlaylistView()
{
    QMap<int, QString> map;
    for ( int i = 0; i < model()->columnCount(); i++ )
    {
        if (!isColumnHidden(i))
        {
            QString column = model()->headerData( i, Qt::Horizontal ).toString();
            int pos = header()->sectionPosition(i);
            map[pos] = column;
        }
    }
    Settings::instance().setPlaylistState(header()->saveState());
}

void PlaylistView::setup()
{
    header()->restoreState(Settings::instance().playlistState());
    header()->setContextMenuPolicy( Qt::ActionsContextMenu );

    for ( int i = 0; i < model()->columnCount(); i++ )
    {
        QAction *action = new QAction( model()->headerData( i, Qt::Horizontal ).toString(), header() );
        header()->addAction( action );
        m_columnActions.append( action );
        action->setCheckable( true );
        if ( !isColumnHidden( i ) )
            action->setChecked( true );
        connect( action, SIGNAL( toggled(bool) ), this, SLOT(toggleColumn(bool) ) );
    }

    setContextMenuPolicy( Qt::ActionsContextMenu );

    QAction *action = new QAction( tr("View Track Details"), header() );
    addAction( action );
    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());
    connect( action, SIGNAL(triggered()), playlist, SLOT(showDetails()));
    connect(playlist, SIGNAL(currentChanged(QModelIndex)), this, SLOT(scrollToIndex(QModelIndex)));
}

void PlaylistView::scrollToIndex(const QModelIndex &index)
{
    scrollTo(index);
}

void PlaylistView::sectionClicked(int section)
{
    if (!isSortingEnabled())
    {
        setSortingEnabled(true);
        sortByColumn(section);
    }
}

void PlaylistView::toggleColumn(bool toggled)
{
    int index = m_columnActions.indexOf(qobject_cast< QAction* >(sender()));
    if ( index != -1 )
    {
        if (toggled)
            showColumn(index);
        else
            hideColumn(index);
    }
}

void PlaylistView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void PlaylistView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
    QTreeView::dragMoveEvent(event);
}

void PlaylistView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        int row = indexAt(event->pos()).row();
        Qt::DropAction action;
        if (event->source() == this) {
            action = Qt::MoveAction;
        } else {
            action = event->proposedAction();
        }
        model()->dropMimeData(event->mimeData(), action, row, 0, QModelIndex());
        event->setDropAction(action);
        event->accept();
    } else {
        event->ignore();
    }
}

void PlaylistView::selectAll()
{
    QTreeView::selectAll();

    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());
    for (int i=0; i<playlist->rowCount(); ++i)
    {
        playlist->setSelected(i, true);
    }
}

void PlaylistView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        startPos = e->pos();

    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());
    int row = indexAt(e->pos()).row();
    if (playlist->rowCount() > row)
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
                for (int j = m_anchor_row; j <= m_pressed_row; j++)
                {
                    playlist->setSelected(j, true);
                }
            }
            else
            {
                playlist->clearSelection();
                for (int j = m_anchor_row; j >= m_pressed_row; j--)
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
    QTreeView::mousePressEvent(e);
}

void PlaylistView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton & (event->modifiers() == Qt::NoModifier))
    {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            startDrag(model()->supportedDragActions());
    }
}

void PlaylistView::startDrag(Qt::DropActions supportedActions)
{
    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());
    QList<PlayListItem*> items;
    foreach(QModelIndex index, selectedIndexes())
    {
        items.append(playlist->item(index.row()));
    }

    if (items.count())
    {
        QMimeData *mimeData = model()->mimeData(selectedIndexes());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec(supportedActions, Qt::CopyAction);
    }
}

QList<int> PlaylistView::selectedRows()
{
    QList<int> rowList;
    foreach(QModelIndex rowItem, selectedIndexes())
        rowList.push_back(rowItem.row());
    return rowList;
}
