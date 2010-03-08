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

#include <qmmp/soundcore.h>
#include <qmmp/decoder.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>

#include "abstractplaylistmodel.h"
#include "mainwindow.h"
#include "settingsdialog.h"
#include "settings.h"
#include "configdialog.h"
#include "volumetoolbutton.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    ui.setupUi(this);
    //qmmp objects
    createTrayIcon();
    m_player = new MediaPlayer(this);
    m_core = new SoundCore(this);
    m_model = new PlayListModel(this);
    m_player->initialize(m_core, m_model);
    new PlaylistParser(this);
    //connections
    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.actionPlay, SIGNAL(triggered()), m_player, SLOT(play()));
    connect(ui.actionPause, SIGNAL(triggered()), m_core, SLOT(pause()));
    connect(ui.actionNext, SIGNAL(triggered()), m_player, SLOT(next()));
    connect(ui.actionPrevious, SIGNAL(triggered()), m_player, SLOT(previous()));
    connect(ui.actionStop, SIGNAL(triggered()), m_player, SLOT(stop()));
    connect(ui.actionOpen, SIGNAL(triggered()),SLOT(addFiles()));
    connect(ui.actionClear, SIGNAL(triggered()),m_model,SLOT(clear()));
    connect(ui.actionRemove, SIGNAL(triggered()), this, SLOT(removeSelected()));
    connect(ui.actionSettings, SIGNAL(triggered()), SLOT(settings()));
    connect(ui.actionQmmpSettings, SIGNAL(triggered()), this, SLOT(settingsQmmp()));
    connect(ui.actionSelectAll, SIGNAL(triggered()), ui.tableView, SLOT(selectAll()));
    connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(updatePosition(qint64)));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(showBitrate(int)));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	     this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    AbstractPlaylistModel *m = new AbstractPlaylistModel(m_model, this);
    const int rowHeight = fontMetrics().height() + 2;
    ui.tableView->verticalHeader()->setDefaultSectionSize(rowHeight);

    ui.tableView->verticalHeader()->setStyleSheet(
     "QHeaderView::section {"
        "padding-bottom: 0px;"
        "padding-top: 0px;"
        "padding-left: 0px;"
        "padding-right: 1px;"
        "margin: 0px;"
     "}"
    );

    ui.tableView->setDragEnabled(true);
    ui.tableView->setAcceptDrops(true);
    ui.tableView->setDragDropMode(QAbstractItemView::InternalMove);
    ui.tableView->resizeColumnsToContents();

    ui.tableView->setModel(m);

    model = new QFileSystemModel(ui.treeView);
    model->setFilter(QDir::AllEntries|QDir::AllDirs|QDir::NoDotAndDotDot);
    model->setNameFilters(QStringList()<<"*.mp3"<<"*.wma"<<"*.flac");
    model->setNameFilterDisables(false);
    model->setReadOnly(true);
    model->setRootPath(Settings::instance().rootFSCollectionDirectory());

    ui.treeView->setModel(model);
    ui.treeView->setRootIndex(model->index(Settings::instance().rootFSCollectionDirectory()));
    ui.treeView->hideColumn(1);
    ui.treeView->hideColumn(2);
    ui.treeView->hideColumn(3);
    connect(m_model, SIGNAL(listChanged()), ui.tableView, SLOT(reset()));
    connect(ui.tableView, SIGNAL(doubleClicked (const QModelIndex &)),
                                SLOT (playSelected(const QModelIndex &)));

    connect(ui.treeView, SIGNAL(doubleClicked (const QModelIndex &)),
            SLOT(addDirectory(const QModelIndex &)));

    VolumeToolButton *volumeButton = new VolumeToolButton(this);
    connect(volumeButton, SIGNAL(volumeChanged(int)), this, SLOT(changeVolume(int)));
    volumeButton->setText("volume");
    ui.toolBar->addWidget(volumeButton);

    m_slider = new QSlider (Qt::Horizontal, this);
    m_label = new QLabel(this);
    m_label->setText("--:--/--:--");
    ui.toolBar->addWidget(m_slider);
    ui.toolBar->addWidget(m_label);

    connect(m_slider, SIGNAL(sliderReleased()), SLOT(seek()));

    trayIcon->setVisible(true);
    trayIcon->show();

}
void MainWindow::changeVolume(int delta)
{
    m_core->setVolume(m_core->leftVolume() + delta/12, m_core->rightVolume() + delta/12);
}

void MainWindow::removeSelected()
{
    m_model->removeSelected();
    // after removeSelected() call selection of table view will be cleared
    // but in the model it will contain 1 element
    // TODO: is it worth to implement proper behaviour with QItemSelectionModel?
    foreach(int row, m_model->getSelectedRows())
	ui.tableView->selectRow(row);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
 {
     switch (reason) {
     case QSystemTrayIcon::Trigger:
     case QSystemTrayIcon::DoubleClick:
	 if (isVisible())
	    hide();
	 else
	    show();
	 break;
     case QSystemTrayIcon::MiddleClick:
	 if (m_core->state() == Qmmp::Paused || m_core->state() == Qmmp::Stopped)
	     m_player->play();
	 else if(m_core->state() == Qmmp::Playing)
	     m_core->pause();
	 break;
     default:
	 ;
     }
 }

void MainWindow::quit()
{
    qApp->quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
	hide();
	event->ignore();
    }
}

void MainWindow::settings()
{
    SettingsDialog settings;
    settings.exec();
}

void MainWindow::settingsQmmp()
{
    ConfigDialog dialog;
    dialog.exec();
}

void MainWindow::addDirectory(const QModelIndex &index)
{
    if (model->fileInfo(index).isDir())
    {
	QString path = model->fileInfo(index).absoluteFilePath();
	qDebug() << QString("addDirectory('%1')").arg(path);
	m_model->addDirectory(path);
    }
    else
	m_model->addFile(model->fileInfo(index).absoluteFilePath());
}

MainWindow::~MainWindow()
{
}

void MainWindow::addFiles()
{
    QString lastDir;
    QStringList filters;
    filters << tr("All Supported Bitstreams")+" (" + Decoder::nameFilters().join (" ") +")";
    filters << Decoder::filters();
    FileDialog::popup(this, FileDialog::AddDirsFiles, &lastDir,
                            m_model, SLOT(addFileList(const QStringList&)),
                            tr("Select one or more files to open"), filters.join(";;"));
}

void MainWindow::playSelected(const QModelIndex &i)
{
    m_player->stop();
    m_model->setCurrent(i.row());
    m_player->play();
    foreach(int row, m_model->getSelectedRows())
	ui.tableView->selectRow(row);
}

void MainWindow::updatePosition(qint64 pos)
{
    m_slider->setMaximum(m_core->totalTime()/1000);
    if(!m_slider->isSliderDown())
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
    switch((int) state)
    {
    case Qmmp::Playing:
	trayIcon->setIcon(QIcon(":/images/play.png"));
        ui.statusbar->showMessage(tr("Playing"));
        if(m_label->text() != "--:--/--:--")
            showBitrate(m_core->bitrate());
        break;
    case Qmmp::Paused:
	trayIcon->setIcon(QIcon(":/images/pause.png"));
        ui.statusbar->showMessage(tr("Paused"));
        break;
    case Qmmp::Stopped:
	trayIcon->setIcon(QIcon(":/images/stop.png"));
	ui.statusbar->showMessage(tr("Stopped"));
        m_label->setText("--:--/--:--");
        m_slider->setValue(0);
        break;
    }

}

void MainWindow::showBitrate(int)
{
    ui.statusbar->showMessage(QString(tr("Playing [%1 kbps/%2 bit/%3]")).arg(m_core->bitrate())
                                    .arg(m_core->precision())
                                    .arg(m_core->channels() > 1 ? tr("Stereo"):tr("Mono")));
}

 void MainWindow::createTrayIcon()
 {
     trayIconMenu = new QMenu(this);
     trayIconMenu->addAction(ui.actionPrevious);
     trayIconMenu->addAction(ui.actionPlay);
     trayIconMenu->addAction(ui.actionPause);
     trayIconMenu->addAction(ui.actionStop);
     trayIconMenu->addAction(ui.actionNext);
     trayIconMenu->addSeparator();
     trayIconMenu->addAction(ui.actionQuit);

     trayIcon = new QSystemTrayIcon(this);
     trayIcon->setContextMenu(trayIconMenu);
     trayIcon->setIcon(QIcon(":/images/stop.png"));
 }
