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
#include <QDebug>
#include <QDockWidget>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QPushButton>
#include <QSettings>
#include <QSignalMapper>
#include <QSlider>
#include <QxtGlobalShortcut>

#include <qmmp/decoder.h>
#include <qmmp/metadatamanager.h>
#include <qmmp/soundcore.h>
#include <qmmpui/configdialog.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/uihelper.h>

#include "abstractplaylistmodel.h"
#include "eqdialog.h"
#include "extendedfilesystemmodel.h"
#include "mainwindow.h"
#include "scrollingtrayicon.h"
#include "settings.h"
#include "settingswidget.h"
#include "trackslider.h"
#include "visualmenu.h"
#include "volumetoolbutton.h"

static const QString defaultTitle = "Qsmmp";

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.playlistPanel->hide();

    if (QSystemTrayIcon::isSystemTrayAvailable())
        QApplication::setQuitOnLastWindowClosed(!Settings::instance().hideOnClose());

    //prepare libqmmp and libqmmpui libraries for usage
    m_player = MediaPlayer::instance();
    m_core = SoundCore::instance();
    m_manager = PlayListManager::instance();
    m_uiHelper = UiHelper::instance();
    m_model = m_manager->currentPlayList();
    m_sortMapper = new QSignalMapper(this);
    createTrayIcon();

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
    connect(ui.actionRemove, SIGNAL(triggered()), ui.playlistView, SLOT(removeSelected()));
    connect(ui.actionSettings, SIGNAL(triggered()), SLOT(showSettings()));
    connect(ui.actionSelectAll, SIGNAL(triggered()), ui.playlistView, SLOT(selectAll()));
    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(updatePosition(qint64)));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(showBitrate(int)));
    connect(ui.lockButton, SIGNAL(clicked(bool)), this, SLOT(lockFSCollectionRoot(bool)));
    connect(ui.actionEqualizer, SIGNAL(triggered()), this, SLOT(showEQ()));
    connect(ui.actionClear, SIGNAL(triggered()),m_model, SLOT(clear()));
    connect(ui.actionShuffle, SIGNAL(triggered()), m_model, SLOT(randomizeList()));
    connect(m_model, SIGNAL(listChanged()), ui.playlistView, SLOT(reset()));
    connect(m_core, SIGNAL(metaDataChanged()), SLOT(currentChanged()));
    connect(ui.shuffleButton, SIGNAL(clicked()), ui.actionShuffle, SLOT(trigger()));
    connect(ui.actionRemoveFSItem, SIGNAL(triggered()), SLOT(removeFSItem()));
    connect(ui.actionRenameFSItem, SIGNAL(triggered()), SLOT(renameFSItem()));

    mapSortAction(ui.actionSortByTitle,    PlayListModel::TITLE);
    mapSortAction(ui.actionSortByAlbum,    PlayListModel::ALBUM);
    mapSortAction(ui.actionSortByArtist,   PlayListModel::ARTIST);
    mapSortAction(ui.actionSortByFilename, PlayListModel::FILENAME);
    mapSortAction(ui.actionSortByPathAndFilename, PlayListModel::PATH_AND_FILENAME);
    mapSortAction(ui.actionSortByDate,     PlayListModel::DATE);
    mapSortAction(ui.actionSortByTrack,    PlayListModel::TRACK);
    connect(m_sortMapper, SIGNAL(mapped(int)), this, SLOT(sortBy(int)));

    createGlobalShortcuts();

    m_visMenu = new VisualMenu(this);
    ui.actionVisualization->setMenu(m_visMenu);
    Visual::initialize(this, m_visMenu, SLOT(updateActions()));

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

    ui.treeView->setModel(m_fsmodel);
    ui.treeView->setRootIndex(rootIndex);
    ui.treeView->hideColumn(1);
    ui.treeView->hideColumn(2);
    ui.treeView->hideColumn(3);
    ui.treeView->addActions(QList<QAction*>() << ui.actionRemoveFSItem << ui.actionRenameFSItem);

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
    connect(m_manager, SIGNAL(currentPlayListChanged(PlayListModel*,PlayListModel*)), SLOT(currentPlayListChanged(PlayListModel*,PlayListModel*)));
    connect(m_manager, SIGNAL(currentPlayListChanged(PlayListModel*,PlayListModel*)), m, SLOT(currentPlayListChanged(PlayListModel*,PlayListModel*)));

    connect(m_uiHelper, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(m_trayIcon, SIGNAL(wheelScrolled(int)), m_core, SLOT(changeVolume(int)));

    m_trayIcon->show();
    setVisible(!Settings::instance().startHidden() || !m_uiHelper->visibilityControl());
}

void MainWindow::lockFSCollectionRoot(bool checked)
{
    QModelIndex rootIndex;
    if (!checked)
    {
        ui.lockButton->setText(tr("Lock"));
        rootIndex = m_fsmodel->setRootPath("/");
    }
    else
    {
        ui.lockButton->setText(tr("Unlock"));
        QModelIndex currentIndex = ui.treeView->currentIndex();
        rootIndex = m_fsmodel->setRootPath(m_fsmodel->filePath(currentIndex));
    }
    updateFSCollectionPath();
    ui.treeView->setRootIndex(rootIndex);
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

void MainWindow::showSettings()
{
    ConfigDialog *confDialog = new ConfigDialog(this);
    SettingsWidget *widget = new SettingsWidget(this);
    confDialog->addPage(tr("Appearance"), widget, QIcon(":/images/interface.png"));
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
    m_uiHelper->addFile();
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
                              .arg(m_core->sampleSize())
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

void MainWindow::currentChanged()
{
    QString title(defaultTitle);
    if (m_model->currentTrack())
        title += " - " + m_model->currentTrack()->formattedTitle();
    setWindowTitle(title);
}

void MainWindow::currentPlayListChanged(PlayListModel *current,PlayListModel *previous)
{
    m_model = current;

    if (previous)
    {
        disconnect(ui.actionClear, SIGNAL(triggered()), previous, SLOT(clear()));
        disconnect(previous, SIGNAL(listChanged()), ui.playlistView, SLOT(reset()));
        disconnect(ui.actionShuffle, SIGNAL(triggered()), previous, SLOT(randomizeList()));
    }

    connect(ui.actionClear, SIGNAL(triggered()),current,SLOT(clear()));
    connect(current, SIGNAL(listChanged()), ui.playlistView, SLOT(reset()));
    connect(ui.actionShuffle, SIGNAL(triggered()), current, SLOT(randomizeList()));
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

    if (QMessageBox::Yes != QMessageBox::question(this, tr("Delete Files"),
                                                  tr("Do you really want to delete this item?"),
                                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        return;

    bool result = m_fsmodel->isDir(index)
            ? m_fsmodel->rmdir(index)
            : m_fsmodel->remove(index);

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

void MainWindow::mapSortAction(QAction *action, int mode)
{
    m_sortMapper->setMapping(action, mode);
    connect(action, SIGNAL(triggered()), m_sortMapper, SLOT(map()));
}

void MainWindow::sortBy(int mode)
{
    m_model->sort(mode);
}

void MainWindow::createTrayIcon()
{
    m_trayIcon = new ScrollingTrayIcon(this);
    m_trayIcon->setIcon(windowIcon());
    QMenu *menu = new QMenu(this);
    menu->addAction(ui.actionOpen);
    menu->addAction(ui.actionPrevious);
    menu->addAction(ui.actionPlay);
    menu->addAction(ui.actionPause);
    menu->addAction(ui.actionStop);
    menu->addAction(ui.actionNext);
    menu->addSeparator();
    menu->addAction(ui.actionSettings);
    menu->addAction(ui.actionQuit);
    m_trayIcon->setContextMenu(menu);
}

void MainWindow::createGlobalShortcuts()
{
    createGlobalShortcut("Ctrl+Alt+Z", m_player, SLOT(previous()));
    createGlobalShortcut("Ctrl+Alt+X", m_player, SLOT(play()));
    createGlobalShortcut("Ctrl+Alt+C", m_core,   SLOT(pause()));
    createGlobalShortcut("Ctrl+Alt+V", m_player, SLOT(stop()));
    createGlobalShortcut("Ctrl+Alt+B", m_player, SLOT(next()));
    createGlobalShortcut("Ctrl+Alt+Up",   this,  SLOT(volumeUp()));
    createGlobalShortcut("Ctrl+Alt+Down", this,  SLOT(volumeDown()));
}

void MainWindow::createGlobalShortcut(const QString &key, const QObject *receiver, const char *member)
{
    QxtGlobalShortcut* shortcut = new QxtGlobalShortcut(this);
    shortcut->setShortcut(QKeySequence(key));
    connect(shortcut, SIGNAL(activated()), receiver, member);
}

void MainWindow::volumeDown()
{
    m_core->changeVolume(-3);
}

void MainWindow::volumeUp()
{
    m_core->changeVolume( 3);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        setVisible(!isVisible());
        if (isVisible()) {
            showNormal();
            raise();
        }
        break;
    default:
        ;
    }
}
