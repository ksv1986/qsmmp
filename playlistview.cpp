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
    Settings::instance().setPlaylistVisibleColumns(map.values());
}

void PlaylistView::setup()
{
    QStringList columns = Settings::instance().playlistVisibleColumns();
    if (columns.count() > 0)
    {
        // setting up column visibility
        for ( int i = 0; i < model()->columnCount(); i++ )
        {
            QString column = model()->headerData( i, Qt::Horizontal ).toString();
            if (columns.contains(column))
                showColumn(i);
            else
                hideColumn(i);
        }
        // setting up column order
        for (int i = 0; i<columns.count(); i++)
        {
            int index;
            for (index = 0; index < model()->columnCount(); index++)
            {
                QString column = model()->headerData( index, Qt::Horizontal ).toString();
                if (column == columns[i])
                    break;
            }
            int vi = header()->visualIndex(index);
            header()->moveSection(vi, i + 1);
        }
    }

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
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void PlaylistView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void PlaylistView::dropEvent(QDropEvent* event)
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

void PlaylistView::selectAll()
{
    QTreeView::selectAll();

    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());
    for (int i=0; i<playlist->count(); ++i)
    {
        playlist->setSelected(i, true);
    }
}

void PlaylistView::mousePressEvent(QMouseEvent *e)
{
    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());

    int row = indexAt(e->pos()).row();

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

void PlaylistView::mouseReleaseEvent(QMouseEvent *e)
{
    AbstractPlaylistModel *playlist = qobject_cast<AbstractPlaylistModel*>(model());

    int row = indexAt(e->pos()).row();

    if ((e->modifiers() == Qt::NoModifier) && playlist->count() > row)
    {
        m_pressed_row = row;

        if (m_pressed_row > m_anchor_row)
        {
            for (int j = m_anchor_row; j <= m_pressed_row; j++)
            {
                playlist->setSelected(j, true);
            }
        }
        else
        {
            for (int j = m_anchor_row; j >= m_pressed_row; j--)
            {
                playlist->setSelected(j, true);
            }
        }

        if (playlist->getSelection(m_pressed_row).count() == 1)
            m_anchor_row = m_pressed_row;

        update();
    }
    QTreeView::mouseReleaseEvent(e);
}

QList<int> PlaylistView::selectedRows()
{
    QList<int> rowList;
    foreach(QModelIndex rowItem, selectedIndexes())
        rowList.push_back(rowItem.row());
    return rowList;
}
