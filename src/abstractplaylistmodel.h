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

#include <QAbstractListModel>

class PlayListModel;
class PlayListTrack;
class SimpleSelection;

class AbstractPlaylistModel : public QAbstractListModel
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

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual Qt::DropActions supportedDragActions() const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                              int column, const QModelIndex &parent);

    void addItem(const QString& path);
    void insertItem(const QString& path, int row);
    void removeItem(PlayListTrack *item);
    PlayListTrack *item(int row);
    void removeAt(int row);

    void clearSelection();
    bool isSelected(int row);
    void setSelected(int row, bool selected);
    void selectAll();
    const SimpleSelection& getSelection(int row);

    void setPlaylist(PlayListModel *model);

signals:
    void currentChanged(const QModelIndex& index);

public slots:
    void listChanged();
    void showDetails();
    void openDirectory();
    void currentPlayListChanged(PlayListModel *current, PlayListModel *previous);

private slots:
    void currentChanged();

private:
    QString formatTime(qint64 time) const;
    PlayListTrack *selectedTrack() const;

    PlayListModel *m_pl;
    mutable QList<PlayListTrack*> itemsToMove;
};

#endif // ABSTRACTPLAYLISTMODEL_H
