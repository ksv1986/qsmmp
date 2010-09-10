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
#include <QFileInfo>
#include <qmmpui/playlistitem.h>
#include "abstractplaylistmodel.h"

AbstractPlaylistModel::AbstractPlaylistModel(PlayListModel *pl, QObject *parent) : QAbstractTableModel(parent)
{
    m_pl = pl;
    connect(m_pl, SIGNAL(listChanged()), this, SLOT(listChanged()));
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

QVariant AbstractPlaylistModel::data (const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && index.row () < m_pl->count())
    {
        PlayListItem *item = m_pl->item(index.row ());

        if (index.column() == 0)
            return item->track();
        if (index.column() == 1)
            return item->artist();
        if (index.column() == 2)
            return item->title().isEmpty() ? item->url() : item->title();
        if (index.column() == 3)
            return item->year();
        if (index.column() == 4)
            return item->album();
    }
    else if (role == Qt::FontRole)
    {
        QFont font;
        if (index.row () == m_pl->currentRow())
            font.setBold(TRUE);
        return font;
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
    else if (role == Qt::FontRole)
    {
        return QFont();
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
    QFileInfo file(path);
    if (file.isDir())
    {
        m_pl->addDirectory(path);
    }
    else
    {
        m_pl->addFile(path);
    }
    this->reset();
}

void AbstractPlaylistModel::sort(int column, Qt::SortOrder)
{
    int mode = 0;
    switch(column)
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
