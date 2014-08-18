#include "playlistbar.h"
#include "settings.h"

#include <qmmpui/playlistmanager.h>

#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>

static PlayListManager* manager() { return PlayListManager::instance(); }

PlaylistBar::PlaylistBar(QWidget *parent=0)
        : QTabBar(parent)
{
    setTabsClosable(true);
    updatePlaylists();
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(removePlaylist(int)));
    connect(manager(), SIGNAL(playListsChanged()), this, SLOT(updatePlaylists()));
}

void PlaylistBar::setPlaylist(int index)
{
    manager()->selectPlayList(index);
    manager()->activatePlayList(index);
}

void PlaylistBar::updatePlaylists()
{
    disconnect(this, SIGNAL(currentChanged(int)), this, SLOT(setPlaylist(int)));
    while (count())
        removeTab(0);

    foreach(PlayListModel *model, manager()->playLists())
        addTab(model->name());

    setCurrentIndex(manager()->currentPlayListIndex());
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(setPlaylist(int)));
}

void PlaylistBar::renamePlaylist(int index)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter playlist name"),
                                         tr("Playlist name:"), QLineEdit::Normal,
                                         tabText(index), &ok);
    if (ok && !text.isEmpty())
        manager()->playListAt(index)->setName(text);
}

void PlaylistBar::removePlaylist(int index)
{
    if (QMessageBox::Ok !=
        QMessageBox::question(this, tr("Remove playlist"),
                              tr("Remove playlist ") + tabText(index) + "?",
                              QMessageBox::Ok, QMessageBox::Cancel))
        return;

    PlayListModel *model = manager()->playListAt(index);
    bool needToSelectNewPlaylist = model == manager()->currentPlayList();

    manager()->removePlayList(model);
    if (needToSelectNewPlaylist)
    {
        if (index >= manager()->playLists().count())
            index =  manager()->playLists().count() - 1;

        setPlaylist(index);
    }
}

void PlaylistBar::newPlaylist()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter new playlist name"),
                                         tr("Playlist name:"), QLineEdit::Normal,
                                         tr("My playlist"), &ok);
    if (ok && !text.isEmpty())
        setPlaylist(manager()->indexOf(manager()->createPlayList(text)));
}

void PlaylistBar::mouseDoubleClickEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
    {
        int index = tabAt(e->pos());
        if (index < 0)
            emit newPlaylist();
        else
            emit renamePlaylist(index);
    }
    QTabBar::mouseDoubleClickEvent(e);
}
