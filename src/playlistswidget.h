#ifndef PLAYLISTSWIDGET_H
#define PLAYLISTSWIDGET_H

#include <QListWidget>

#include <qmmpui/playlistmanager.h>

class PlaylistsWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit PlaylistsWidget(QWidget *parent = 0);

private slots:
    void playlistsWidgetItemChanged(QListWidgetItem*);
    void playlistsWidgetContextMenuRequested(QPoint);
    void setPlaylist(QModelIndex index);
    void updatePlaylists();
    void renamePlaylist();
    void removePlaylist();
    void newPlaylist();

private:
    void createActions();
    void setPlaylist(int index);
    PlayListManager* manager() { return PlayListManager::instance(); }
};

#endif // PLAYLISTSWIDGET_H
