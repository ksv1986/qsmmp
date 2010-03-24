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
#include <qmmpui/generalhandler.h>

#include "abstractplaylistmodel.h"
#include "mainwindow.h"
#include "settings.h"
#include "configdialog.h"
#include "volumetoolbutton.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    ui.setupUi(this);
    //qmmp objects
    m_player = new MediaPlayer(this);
    m_core = new SoundCore(this);
    m_model = new PlayListModel(this);
    m_player->initialize(m_core, m_model);
    new PlaylistParser(this);
    m_generalHandler = new GeneralHandler(this);
    //connections
    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.actionPlay, SIGNAL(triggered()), m_player, SLOT(play()));
    connect(ui.actionPause, SIGNAL(triggered()), m_core, SLOT(pause()));
    connect(ui.actionNext, SIGNAL(triggered()), m_player, SLOT(next()));
    connect(ui.actionPrevious, SIGNAL(triggered()), m_player, SLOT(previous()));
    connect(ui.actionStop, SIGNAL(triggered()), m_player, SLOT(stop()));
    connect(ui.actionOpen, SIGNAL(triggered()),SLOT(addFiles()));
    connect(ui.actionClear, SIGNAL(triggered()),m_model,SLOT(clear()));
    connect(ui.clearButton, SIGNAL(clicked()), ui.actionClear, SLOT(trigger()));
    connect(ui.actionRemove, SIGNAL(triggered()), this, SLOT(removeSelected()));
    connect(ui.actionSettings, SIGNAL(triggered()), SLOT(settings()));
    connect(ui.actionSelectAll, SIGNAL(triggered()), ui.tableView, SLOT(selectAll()));
    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(updatePosition(qint64)));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(showBitrate(int)));
    connect(ui.lockButton, SIGNAL(clicked(bool)), this, SLOT(lockFSCollectionRoot(bool)));
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

    ui.tableView->setModel(m);
    ui.tableView->setup();
    ui.tableView->setColumnWidth(0, 30);
    ui.tableView->setColumnWidth(1, 100);
    ui.tableView->setColumnWidth(2, 200);
    ui.tableView->setColumnWidth(3, 45);

    model = new QFileSystemModel(ui.treeView);
    model->setFilter(QDir::AllEntries|QDir::AllDirs|QDir::NoDotAndDotDot);

    QStringList list = Decoder::filters();
    QStringList filters;
    QRegExp rx("(\\*.\\w+)[\\s\\)]");
    foreach(QString str, list){
	int pos = 0;

	while ((pos = rx.indexIn(str, pos)) != -1) {
	    filters << rx.cap(1);
	    pos += rx.matchedLength();
	}
    }
    model->setNameFilters(filters);
    model->setNameFilterDisables(false);
    model->setReadOnly(true);
    model->setRootPath(Settings::instance().rootFSCollectionDirectory());
    ui.fsCollectionPathLabel->setText(model->rootPath());

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

    VolumeToolButton *volumeButton = new VolumeToolButton(this, m_core->leftVolume(), 0, 100);
    // should be connected in both ways
    connect(volumeButton, SIGNAL(volumeChanged(int)), this, SLOT(changeVolume(int)));
    ui.toolBar->addWidget(volumeButton);

    m_slider = new QSlider (Qt::Horizontal, this);
    m_label = new QLabel(this);
    m_label->setText("--:--/--:--");
    ui.toolBar->addWidget(m_slider);
    ui.toolBar->addWidget(m_label);

    connect(m_slider, SIGNAL(sliderReleased()), SLOT(seek()));

    connect(m_generalHandler, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    connect(m_generalHandler, SIGNAL(exitCalled()), qApp, SLOT(quit()));

    setVisible(!Settings::instance().startHidden() || !m_generalHandler->visibilityControl());
}

void MainWindow::changeVolume(int delta)
{
    m_core->setVolume(m_core->leftVolume() + delta, m_core->rightVolume() + delta);
}

void MainWindow::lockFSCollectionRoot(bool checked)
{
    if(!checked)
    {
	ui.lockButton->setText(tr("Lock"));
	model->setRootPath("/");
    }
    else
    {
	ui.lockButton->setText(tr("Unlock"));
	model->setRootPath(model->filePath(ui.treeView->currentIndex()));
    }
    ui.fsCollectionPathLabel->setText(model->rootPath());
    ui.treeView->setRootIndex(model->index(model->rootPath()));
    Settings::instance().setRootFSCollectionDirectory(model->rootPath());
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

void MainWindow::toggleVisibility()
 {
    setVisible(!isVisible());
 }

void MainWindow::settings()
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
        ui.statusbar->showMessage(tr("Playing"));
        if(m_label->text() != "--:--/--:--")
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
    }

}

void MainWindow::showBitrate(int)
{
    ui.statusbar->showMessage(QString(tr("Playing [%1 kbps/%2 bit/%3]")).arg(m_core->bitrate())
                                    .arg(m_core->precision())
                                    .arg(m_core->channels() > 1 ? tr("Stereo"):tr("Mono")));
}
