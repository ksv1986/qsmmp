/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <qmmp/qmmp.h>
#include <ui_mainwindow.h>

class QLabel;
class QFileSystemModel;

class PlayListModel;
class MediaPlayer;
class SoundCore;
class VisualMenu;
class PlayListManager;
class TrackSlider;
class UiHelper;
class RecursiveSortFilterProxyModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void moveEvent(QMoveEvent *event);

    ~MainWindow();

private slots:
    void addFiles();
    void playSelected(const QModelIndex &i);
    void removeSelected();
    void updatePosition(qint64 pos);
    void seek();
    void showState(Qmmp::State);
    void showBitrate(int);
    void settings();
    void lockFSCollectionRoot(bool checked);
    void toggleVisibility();
    void showEQ();
    void setPlaylist(QModelIndex index);
    void updatePlaylists();
    void playlistsWidgetContextMenuRequested(QPoint point);
    void playlistsWidgetItemChanged(QListWidgetItem *item);
    void renamePlaylist();
    void removePlaylist();
    void newPlaylist();
    void shufflePlaylist();
    void updateFSCollectionPath();
    void removeFSItem();
    void renameFSItem();
    void filterTextChanged(QString filterText);

private:
    void setPlaylist(int index);

    PlayListModel *m_model;
    Ui::MainWindow ui;
    MediaPlayer *m_player;
    TrackSlider *m_slider;
    QLabel *m_label;
    SoundCore *m_core;
    QFileSystemModel *m_fsmodel;
    RecursiveSortFilterProxyModel *m_proxyModel;
    VisualMenu *m_visMenu;
    PlayListManager *m_manager;
    QString m_lastDir;
    UiHelper *m_uiHelper;
};

#endif
