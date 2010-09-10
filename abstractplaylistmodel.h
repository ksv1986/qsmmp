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

#ifndef ABSTRACTPLAYLISTMODEL_H
#define ABSTRACTPLAYLISTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <qmmpui/playlistmodel.h>

class AbstractPlaylistModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AbstractPlaylistModel(PlayListModel *pl, QObject *parent);
    ~AbstractPlaylistModel();

    virtual int columnCount (const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual void sort(int column, Qt::SortOrder order);
    void addItem(const QString& path);

    void clearSelection() {
        m_pl->clearSelection();
    }
    bool isSelected(int row) {
        return m_pl->isSelected(row);
    }
    void setSelected(int row, bool selected) {
        m_pl->setSelected(row, selected);
    }
    int count() {
        return m_pl->count();
    }
    const SimpleSelection& getSelection(int row) {
        return m_pl->getSelection(row);
    }

    void setPlaylist(PlayListModel *model) {
        m_pl = model;
        listChanged();
    }

public slots:
    void listChanged();
    void showDetails();
private:
    QString formatTime(qint64 time) const;
    PlayListModel *m_pl;
};

#endif // ABSTRACTPLAYLISTMODEL_H
