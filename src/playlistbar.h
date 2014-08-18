#ifndef PLAYLISTBAR_H
#define PLAYLISTBAR_H

#include <QTabBar>

class PlaylistBar : public QTabBar
{
    Q_OBJECT
public:
    PlaylistBar(QWidget *parent);
private slots:
    void updatePlaylists();
    void newPlaylist();
    void renamePlaylist(int);
    void removePlaylist(int);
    void setPlaylist(int);
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *);
};

#endif // PLAYLISTBAR_H
