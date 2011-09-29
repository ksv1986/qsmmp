#ifndef RECURSIVESORTFILTERPROXYMODEL_H
#define RECURSIVESORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class RecursiveSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RecursiveSortFilterProxyModel(QObject *parent = 0);
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    void setSourceRoot(const QModelIndex &sourceRoot);

private:
    QModelIndex m_sourceRoot;

};

#endif // RECURSIVESORTFILTERPROXYMODEL_H
