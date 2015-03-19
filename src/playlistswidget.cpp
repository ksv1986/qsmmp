#include <QAction>
#include <QInputDialog>
#include <QMenu>

#include "playlistswidget.h"

PlaylistsWidget::PlaylistsWidget(QWidget *parent) :
    QListWidget(parent)
{
    createActions();
    updatePlaylists();

    connect(manager(), SIGNAL(playListsChanged()), SLOT(updatePlaylists()));
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(setPlaylist(QModelIndex)));
    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(renamePlaylist()));
    connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(playlistsWidgetItemChanged(QListWidgetItem*)));
}

void PlaylistsWidget::createActions()
{
    QAction *newAction = new QAction(tr("New"), this);
    QAction *removeAction = new QAction(tr("Remove"), this);
    QAction *renameAction = new QAction(tr("Rename"), this);

    connect(renameAction, SIGNAL(triggered()), this, SLOT(renamePlaylist()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removePlaylist()));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newPlaylist()));

    addAction(newAction);
    addAction(removeAction);
    addAction(renameAction);
}

void PlaylistsWidget::setPlaylist(QModelIndex index)
{
    setPlaylist(index.row());
}

void PlaylistsWidget::setPlaylist(int index)
{
    manager()->selectPlayList(index);
    manager()->activatePlayList(manager()->selectedPlayList());
}

void PlaylistsWidget::updatePlaylists()
{
    clear();
    foreach(PlayListModel *model, manager()->playLists())
    {
        addItem(model->name());
    }
    setCurrentRow(manager()->selectedPlayListIndex());
}

void PlaylistsWidget::renamePlaylist()
{
    QListWidgetItem* item = currentItem();
    if (item)
    {
        item->setFlags(Qt::ItemIsEditable | item->flags());
        editItem(item);
    }
}

void PlaylistsWidget::playlistsWidgetItemChanged(QListWidgetItem *item)
{
    manager()->playListAt(row(item))->setName(item->text());
}

void PlaylistsWidget::removePlaylist()
{
    int index = currentIndex().row();
    PlayListModel *model = manager()->playListAt(index);
    bool needToSelectNewPlaylist = model == manager()->currentPlayList();

    manager()->removePlayList(model);

    if (needToSelectNewPlaylist)
    {
        if (index >= manager()->playLists().count())
            index = manager()->playLists().count() - 1;

        setPlaylist(index);
    }
}

void PlaylistsWidget::newPlaylist()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter new playlist name"),
                                         tr("Playlist name:"), QLineEdit::Normal,
                                         tr("My playlist"), &ok);
    if (ok && !text.isEmpty())
    {
        PlayListModel *model = manager()->createPlayList(text);
        manager()->selectPlayList(model);
        manager()->activatePlayList(model);
    }
}
