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

#include <QFont>
#include <QChar>
#include <QSet>
#include <QFileInfo>
#include <QMimeData>
#include <QDebug>
#include <QUrl>
#include <QApplication>
#include <QPalette>

#include <qmmpui/playlisttrack.h>

#include "abstractplaylistmodel.h"

AbstractPlaylistModel::AbstractPlaylistModel(PlayListModel *pl, QObject *parent) : QAbstractListModel(parent)
{
    m_pl = pl;
    connect(m_pl, SIGNAL(listChanged()), this, SLOT(listChanged()));
    connect(m_pl, SIGNAL(currentChanged()), this, SLOT(currentChanged()));
}

AbstractPlaylistModel::~AbstractPlaylistModel() {}

int AbstractPlaylistModel::columnCount (const QModelIndex &) const
{
    return 5;
}

void AbstractPlaylistModel::listChanged()
{
    reset();
}

void AbstractPlaylistModel::currentChanged()
{
    int row = m_pl->currentIndex();
    currentChanged(index(row));
}

QVariant AbstractPlaylistModel::data (const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && index.row () < m_pl->count())
    {
        PlayListTrack *item = m_pl->track(index.row ());
        if (index.column() == 0)
            return (*item)[Qmmp::TRACK];
        if (index.column() == 1)
            return (*item)[Qmmp::ARTIST];
        if (index.column() == 2)
            return (*item)[Qmmp::TITLE].isEmpty() ? QFileInfo(item->url()).fileName() : (*item)[Qmmp::TITLE];
        if (index.column() == 3)
            return (*item)[Qmmp::YEAR];
        if (index.column() == 4)
            return (*item)[Qmmp::ALBUM];
    }
    else if (role == Qt::BackgroundRole && index.row () == m_pl->currentIndex())
    {
        return QApplication::palette().brush(QPalette::Current, QPalette::Background);
    }
    else if (role == Qt::ForegroundRole && index.row () == m_pl->currentIndex())
    {
        return QApplication::palette().brush(QPalette::Current, QPalette::Foreground);
    }
    return QVariant();
}

QVariant AbstractPlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == 0)
            return tr("#");
        if (section == 1)
            return tr("Artist");
        if (section == 2)
            return tr("Title");
        if (section == 3)
            return tr("Year");
        if (section == 4)
            return tr("Album");
    }
    else if (role == Qt::DisplayRole)
    {
        return section;
    }
    return QVariant();
}

int AbstractPlaylistModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_pl->count();
}

QString AbstractPlaylistModel::formatTime(qint64 time) const
{
    return QString("%1:%2").arg(time/60).arg(time%60,2,10,QChar('0'));
}

void AbstractPlaylistModel::addItem(const QString& path)
{
    m_pl->add(path);
}

void AbstractPlaylistModel::insertItem(const QString &path, int row)
{
    QFileInfo file(path);
    if (file.isDir())
    {
        m_pl->add(path);
    }
    else
    {
        m_pl->add(path);
        m_pl->clearSelection();
        m_pl->setSelected(m_pl->count() - 1, true);
        m_pl->moveItems(m_pl->count() - 1, row);
    }
}

void AbstractPlaylistModel::sort(int column, Qt::SortOrder)
{
    int mode = 0;
    switch (column)
    {
    case 0:
        mode = PlayListModel::TRACK;
        break;
    case 1:
        mode = PlayListModel::ARTIST;
        break;
    case 2:
        mode = PlayListModel::TITLE;
        break;
    case 3:
        mode = PlayListModel::DATE;
        break;
    case 4:
        mode = PlayListModel::ALBUM;
        break;

    }

    m_pl->sort(mode);
}

void AbstractPlaylistModel::showDetails()
{
    m_pl->showDetails();
}

Qt::ItemFlags AbstractPlaylistModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return Qt::ItemIsDragEnabled
               | Qt::ItemIsDropEnabled
               | QAbstractListModel::flags(index);
    }
    return  Qt::ItemIsDropEnabled
            | QAbstractListModel::flags(index);
}

Qt::DropActions AbstractPlaylistModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions AbstractPlaylistModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList AbstractPlaylistModel::mimeTypes() const
{
    return QStringList() << "text/uri-list";
}

QMimeData *AbstractPlaylistModel::mimeData(const QModelIndexList &indexes) const
{
    QList<int> rows;
    foreach(QModelIndex index, indexes)
    {
        if (index.isValid() && !rows.contains(index.row()))
        {
            rows << index.row();
        }
    }
    qSort(rows);
    QList<QUrl> urls;
    foreach(int row, rows)
    {
        PlayListTrack *item = m_pl->track(row);
        itemsToMove.append(item);
        urls << QUrl(item->url());
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setUrls(urls);
    return mimeData;
}

bool AbstractPlaylistModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
        int row, int column, const QModelIndex &)
{
    Q_UNUSED(column)

    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasUrls())
        return false;

    if (row == -1)
        row = rowCount() - 1;

    if(action == Qt::MoveAction && itemsToMove.count() > 0)
    {
        foreach(PlayListTrack *item, itemsToMove)
        {
            m_pl->clearSelection();
            int index = m_pl->items().indexOf(item);
            m_pl->setSelected(index, true);
            m_pl->moveItems(index, row);
            if (index > row)
                row ++;
        }
    }
    else
    {
        foreach(QUrl url, data->urls())
        {
            if (row >= 0)
                insertItem(url.toLocalFile(), row++);
            else
                addItem(url.toLocalFile());
        }
    }
    itemsToMove.clear();
    return true;
}

PlayListTrack *AbstractPlaylistModel::item(int row)
{
    return m_pl->track(row);
}

void AbstractPlaylistModel::removeAt(int row)
{
    m_pl->removeTrack(row);
}

void AbstractPlaylistModel::removeItem(PlayListTrack *item)
{
    m_pl->removeTrack(item);
}

void AbstractPlaylistModel::clearSelection()
{
    m_pl->clearSelection();
}

bool AbstractPlaylistModel::isSelected(int row)
{
    return m_pl->isSelected(row);
}

void AbstractPlaylistModel::setSelected(int row, bool selected)
{
    m_pl->setSelected(row, selected);
}

void AbstractPlaylistModel::selectAll()
{
    m_pl->selectAll();
}

const SimpleSelection& AbstractPlaylistModel::getSelection(int row)
{
    return m_pl->getSelection(row);
}

void AbstractPlaylistModel::setPlaylist(PlayListModel *model)
{
    m_pl = model;
    listChanged();
}

void AbstractPlaylistModel::currentPlayListChanged(PlayListModel *current, PlayListModel *previous)
{
    Q_UNUSED(previous)
    setPlaylist(current);
}
