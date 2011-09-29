#include "recursivesortfilterproxymodel.h"

RecursiveSortFilterProxyModel::RecursiveSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

void RecursiveSortFilterProxyModel::setSourceRoot(const QModelIndex &sourceRoot)
{
    m_sourceRoot = sourceRoot;
}

bool RecursiveSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);

    if (m_sourceRoot.isValid())
    {
        QModelIndex parent = m_sourceRoot;
        while(parent.isValid())
        {
            if (parent == sourceIndex)
                return true;

            parent = parent.parent();
        }
    }

    QString data = sourceModel()->data(sourceIndex).toString();
    if (data.contains(filterRegExp()))
    {
        return true;
    }

    sourceModel()->fetchMore(sourceIndex);
    for(int row = 0; row < sourceModel()->rowCount(sourceIndex); ++row)
    {
        if (filterAcceptsRow(row, sourceIndex))
        {
            return true;
        }
    }

    return false;
}
