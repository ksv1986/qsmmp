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
#include <QPushButton>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QFileSystemModel>
#include <QDebug>
#include <QKeyEvent>
#include <QList>
#include <QModelIndex>
#include <QDockWidget>
#include <QSettings>
#include <QInputDialog>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include <qmmp/soundcore.h>
#include <qmmp/decoder.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/configdialog.h>

#include "abstractplaylistmodel.h"
#include "mainwindow.h"
#include "settings.h"
#include "volumetoolbutton.h"
#include "visualmenu.h"
#include "eqdialog.h"
#include "trackslider.h"
#include "extendedfilesystemmodel.h"
#include "settingswidget.h"
#include "recursivesortfilterproxymodel.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    ui.setupUi(this);

    if (QSystemTrayIcon::isSystemTrayAvailable())
        QApplication::setQuitOnLastWindowClosed(!Settings::instance().hideOnClose());

    //prepare libqmmp and libqmmpui libraries for usage
    m_player = MediaPlayer::instance();
    m_core = SoundCore::instance();
    m_manager = PlayListManager::instance();
    m_uiHelper = UiHelper::instance();
    m_model = m_manager->currentPlayList();

    //set geometry
    move(Settings::instance().windowGeometry().topLeft());
    resize(Settings::instance().windowGeometry().size());
    //set icons
    if (Settings::instance().useStandardIcons())
    {
        ui.actionPlay->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPlay));
        ui.actionPause->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPause));
        ui.actionNext->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaSkipForward));
        ui.actionPrevious->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaSeekBackward));
        ui.actionStop->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaStop));
    }
    else
    {
        ui.actionPlay->setIcon(QIcon(":/images/play.png"));
        ui.actionPause->setIcon(QIcon(":/images/pause.png"));
        ui.actionNext->setIcon(QIcon(":/images/next.png"));
        ui.actionPrevious->setIcon(QIcon(":/images/previous.png"));
        ui.actionStop->setIcon(QIcon(":/images/stop.png"));
    }
    //connections
    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.actionPlay, SIGNAL(triggered()), m_player, SLOT(play()));
    connect(ui.actionPause, SIGNAL(triggered()), m_core, SLOT(pause()));
    connect(ui.actionNext, SIGNAL(triggered()), m_player, SLOT(next()));
    connect(ui.actionPrevious, SIGNAL(triggered()), m_player, SLOT(previous()));
    connect(ui.actionStop, SIGNAL(triggered()), m_player, SLOT(stop()));
    connect(ui.actionOpen, SIGNAL(triggered()),SLOT(addFiles()));
    connect(ui.clearButton, SIGNAL(clicked()), ui.actionClear, SLOT(trigger()));
    connect(ui.actionRemove, SIGNAL(triggered()), this, SLOT(removeSelected()));
    connect(ui.actionSettings, SIGNAL(triggered()), SLOT(settings()));
    connect(ui.actionSelectAll, SIGNAL(triggered()), ui.playlistView, SLOT(selectAll()));
    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui.actionRenamePlaylist, SIGNAL(triggered()), this, SLOT(renamePlaylist()));
    connect(ui.actionRemovePlaylist, SIGNAL(triggered()), this, SLOT(removePlaylist()));
    connect(ui.actionNewPlaylist, SIGNAL(triggered()), this, SLOT(newPlaylist()));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(updatePosition(qint64)));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(showBitrate(int)));
    connect(ui.lockButton, SIGNAL(clicked(bool)), this, SLOT(lockFSCollectionRoot(bool)));
    connect(ui.actionEqualizer, SIGNAL(triggered()), this, SLOT(showEQ()));
    connect(ui.actionClear, SIGNAL(triggered()),m_model,SLOT(clear()));
    connect(m_model, SIGNAL(listChanged()), ui.playlistView, SLOT(reset()));
    connect(ui.shuffleButton, SIGNAL(clicked()), ui.actionShuffle, SLOT(trigger()));
    connect(ui.actionShuffle, SIGNAL(triggered()), this, SLOT(shufflePlaylist()));
    connect(ui.actionRemoveFSItem, SIGNAL(triggered()), SLOT(removeFSItem()));
    connect(ui.actionRenameFSItem, SIGNAL(triggered()), SLOT(renameFSItem()));

    m_visMenu = new VisualMenu(this);
    ui.actionVisualization->setMenu(m_visMenu);
    Visual::initialize(this, m_visMenu, SLOT(updateActions()));

    ui.playlistView->setDragEnabled(true);
    ui.playlistView->setAcceptDrops(true);
    ui.playlistView->setDropIndicatorShown(false);
    ui.playlistView->setDragDropMode(QAbstractItemView::InternalMove);

    AbstractPlaylistModel *m = new AbstractPlaylistModel(m_model, this);
    ui.playlistView->setModel(m);
    ui.playlistView->setup();

    m_fsmodel = new ExtendedFileSystemModel(ui.treeView);
    m_fsmodel->setFilter(QDir::AllEntries|QDir::AllDirs|QDir::NoDotAndDotDot);

    m_fsmodel->setNameFilters(MetaDataManager::instance()->nameFilters());
    m_fsmodel->setNameFilterDisables(false);
    m_fsmodel->setReadOnly(true);
    m_fsmodel->setRootPath(Settings::instance().rootFSCollectionDirectory());
    updateFSCollectionPath();
    QModelIndex rootIndex =  m_fsmodel->index(Settings::instance().rootFSCollectionDirectory());

    m_proxyModel = new RecursiveSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_fsmodel);
    m_proxyModel->setDynamicSortFilter(true);
    m_proxyModel->setFilterKeyColumn(0);
    m_proxyModel->setSourceRoot(rootIndex);

    ui.treeView->setModel(m_proxyModel);
    ui.treeView->setRootIndex(m_proxyModel->mapFromSource(rootIndex));
    ui.treeView->hideColumn(1);
    ui.treeView->hideColumn(2);
    ui.treeView->hideColumn(3);
    ui.treeView->addActions(QList<QAction*>() << ui.actionRemoveFSItem << ui.actionRenameFSItem);

    connect(ui.filterLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged(QString)));

    connect(ui.playlistView, SIGNAL(doubleClicked (const QModelIndex &)),
            SLOT (playSelected(const QModelIndex &)));

    VolumeToolButton *volumeButton = new VolumeToolButton(m_core->leftVolume(), this, 0, 100);
    connect(volumeButton, SIGNAL(volumeChanged(int, int)), m_core, SLOT(setVolume(int,int)));
    connect(m_core, SIGNAL(volumeChanged(int,int)), volumeButton, SLOT(setVolume(int,int)));
    ui.toolBar->addWidget(volumeButton);

    m_slider = new TrackSlider (this);
    m_label = new QLabel(this);
    m_label->setText("--:--/--:--");
    ui.toolBar->addWidget(m_slider);
    ui.toolBar->addWidget(m_label);

    connect(m_slider, SIGNAL(sliderReleased()), SLOT(seek()));

    connect(m_manager, SIGNAL(playListsChanged()), SLOT(updatePlaylists()));
    updatePlaylists();

    connect(ui.playlistWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(setPlaylist(QModelIndex)));
    connect(ui.playlistWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(playlistsWidgetContextMenuRequested(QPoint)));
    connect(ui.playlistWidget, SIGNAL(doubleClicked(QModelIndex)), ui.actionRenamePlaylist, SLOT(trigger()));
    connect(ui.playlistWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(playlistsWidgetItemChanged(QListWidgetItem*)));

    connect(m_uiHelper, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    connect(m_uiHelper, SIGNAL(exitCalled()), qApp, SLOT(quit()));

    setVisible(!Settings::instance().startHidden() || !m_uiHelper->visibilityControl());
}

void MainWindow::removeSelected()
{
    for (int row = 0; row < m_model->count(); row++)
        m_model->setSelected(row, false);
    foreach(int row, ui.playlistView->selectedRows())
        m_model->setSelected(row, true);
    m_model->removeSelected();
}

void MainWindow::lockFSCollectionRoot(bool checked)
{
    if (!checked)
    {
        ui.lockButton->setText(tr("Lock"));
        m_fsmodel->setRootPath("/");
    }
    else
    {
        ui.lockButton->setText(tr("Unlock"));
        m_fsmodel->setRootPath(m_fsmodel->filePath(ui.treeView->currentIndex()));
    }
    updateFSCollectionPath();
    ui.treeView->setRootIndex(m_fsmodel->index(m_fsmodel->rootPath()));
    Settings::instance().setRootFSCollectionDirectory(m_fsmodel->rootPath());
}

void MainWindow::updateFSCollectionPath()
{
    ui.fsCollectionPathLabel->setText(m_fsmodel->rootPath());
}

void MainWindow::toggleVisibility()
{
    setVisible(!isVisible());
}

void MainWindow::settings()
{
    ConfigDialog *confDialog = new ConfigDialog(this);
    SettingsWidget *widget = new SettingsWidget(this);
    confDialog->addPage(tr("Appearance"), widget, QIcon(":/interface.png"));
    confDialog->exec();
    confDialog->deleteLater();

    widget->applySettings();

    m_visMenu->updateActions();
}

MainWindow::~MainWindow()
{
}

void MainWindow::addFiles()
{
    QStringList filters;
    filters << tr("All Supported Bitstreams")
    + " (" + MetaDataManager::instance()->nameFilters().join (" ") +")";
    filters << MetaDataManager::instance()->filters();
    FileDialog::popup(this, FileDialog::AddDirsFiles, &m_lastDir,
                      m_model, SLOT(add(const QStringList&)),
                      tr("Select one or more files to open"), filters.join(";;"));
}

void MainWindow::playSelected(const QModelIndex &i)
{
    m_player->stop();
    m_model->setCurrent(i.row());
    m_player->play();
}

void MainWindow::updatePosition(qint64 pos)
{
    m_slider->setMaximum(m_core->totalTime()/1000);
    if (!m_slider->isSliderDown())
        m_slider->setValue(pos/1000);
    m_label->setText(QString("%1:%2/%3:%4").arg(pos/1000/60, 2, 10, QChar('0'))
                     .arg(pos/1000%60, 2, 10, QChar('0'))
                     .arg(m_core->totalTime()/1000/60, 2, 10, QChar('0'))
                     .arg(m_core->totalTime()/1000%60, 2, 10, QChar('0')));
}

void MainWindow::seek()
{
    m_core->seek(m_slider->value()*1000);
}

void MainWindow::showState(Qmmp::State state)
{
    switch (state)
    {
    case Qmmp::Playing:
        ui.statusbar->showMessage(tr("Playing"));
        if (m_label->text() != "--:--/--:--")
            showBitrate(m_core->bitrate());
        break;
    case Qmmp::Paused:
        ui.statusbar->showMessage(tr("Paused"));
        break;
    case Qmmp::Stopped:
        ui.statusbar->showMessage(tr("Stopped"));
        m_label->setText("--:--/--:--");
        m_slider->setValue(0);
        break;
    case Qmmp::Buffering:
        ui.statusbar->showMessage(tr("Buffering..."));
        break;
    case Qmmp::NormalError:
        ui.statusbar->showMessage(tr("Error"));
        break;
    case Qmmp::FatalError:
        ui.statusbar->showMessage(tr("Fatal error"));
        break;
    }

}

void MainWindow::showBitrate(int)
{
    ui.statusbar->showMessage(QString(tr("Playing [%1 kbps/%2 bit/%3]"))
                              .arg(m_core->bitrate())
                              .arg(m_core->frequency())
                              .arg(m_core->channels() > 1 ? tr("Stereo"):tr("Mono")));
}

void MainWindow::showEQ()
{
    EQDialog dialog(m_core, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        dialog.writeSettings();
    }
}

void MainWindow::setPlaylist(QModelIndex index)
{
    setPlaylist(index.row());
}

void MainWindow::setPlaylist(int index)
{
    if (m_model)
    {
        disconnect(ui.actionClear, SIGNAL(triggered()),m_model,SLOT(clear()));
        disconnect(m_model, SIGNAL(listChanged()), ui.playlistView, SLOT(reset()));
    }

    m_manager->selectPlayList(index);
    m_model = m_manager->playListAt(index);
    m_manager->activatePlayList(m_model);

    static_cast<AbstractPlaylistModel*>(ui.playlistView->model())->setPlaylist(m_model);

    connect(ui.actionClear, SIGNAL(triggered()),m_model,SLOT(clear()));
    connect(m_model, SIGNAL(listChanged()), ui.playlistView, SLOT(reset()));
    ui.playlistView->reset();
}

void MainWindow::updatePlaylists()
{
    ui.playlistWidget->clear();
    foreach(PlayListModel *model, m_manager->playLists())
    {
        ui.playlistWidget->addItem(model->name());
    }
    int row = m_manager->indexOf(m_manager->selectedPlayList());
    ui.playlistWidget->setCurrentRow (row);
}

void MainWindow::playlistsWidgetContextMenuRequested(QPoint point)
{
    QMenu menu(this);
    menu.addAction(ui.actionNewPlaylist);
    menu.addAction(ui.actionRemovePlaylist);
    menu.addAction(ui.actionRenamePlaylist);
    menu.exec(ui.playlistWidget->mapToGlobal(point));
}

void MainWindow::renamePlaylist()
{
    QListWidgetItem* item = ui.playlistWidget->currentItem();
    if (item)
    {
        item->setFlags(Qt::ItemIsEditable | item->flags());
        ui.playlistWidget->editItem(item);
    }
}

void MainWindow::playlistsWidgetItemChanged(QListWidgetItem *item)
{
    m_manager->playListAt(ui.playlistWidget->row(item))->setName(item->text());
}

void MainWindow::removePlaylist()
{
    int index = ui.playlistWidget->currentIndex().row();
    PlayListModel *model = m_manager->playListAt(index);
    if (m_model == model)
        m_model = NULL;

    m_manager->removePlayList(model);

    if (!m_model)
    {
        if (index >= m_manager->playLists().count())
            index = m_manager->playLists().count() - 1;
        setPlaylist(index);
    }
}

void MainWindow::newPlaylist()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter new playlist name"),
                                         tr("Playlist name:"), QLineEdit::Normal,
                                         tr("My playlist"), &ok);
    if (ok && !text.isEmpty())
    {
        PlayListModel *model = m_manager->createPlayList(text);
        m_manager->activatePlayList(model);
    }
}

void MainWindow::shufflePlaylist()
{
    m_manager->currentPlayList()->randomizeList();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Settings::instance().setWindowGeometry(QRect(pos(), event->size()));
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    Settings::instance().setWindowGeometry(QRect(event->pos(), size()));
}

void MainWindow::removeFSItem()
{
    QModelIndex index = ui.treeView->currentIndex();
    if (!index.isValid())
        return;

    QModelIndex soureIndex = m_proxyModel->mapToSource(index);

    bool result = m_fsmodel->isDir(soureIndex)
            ? m_fsmodel->rmdir(soureIndex)
            : m_fsmodel->remove(soureIndex);

    if (!result)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Failed to remove."));
        msgBox.exec();
    }
}

void MainWindow::renameFSItem()
{
    QModelIndex index = ui.treeView->currentIndex();
    if (!index.isValid())
        return;

    QFileInfo fileInfo = m_fsmodel->fileInfo(index);
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter new file name"),
                                         tr("File name:"), QLineEdit::Normal,
                                         fileInfo.fileName(), &ok);
    if (ok && !text.isEmpty())
    {
        if (!QFile::rename(fileInfo.absoluteFilePath(), fileInfo.absolutePath() + QDir::separator() + text))
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Failed to rename."));
            msgBox.exec();
        }
    }
}

void MainWindow::filterTextChanged(QString filterText)
{
    m_proxyModel->setFilterRegExp(filterText);
}
