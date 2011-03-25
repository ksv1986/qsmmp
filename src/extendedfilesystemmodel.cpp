#include "extendedfilesystemmodel.h"

ExtendedFileSystemModel::ExtendedFileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
}

QVariant ExtendedFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
    {
        return QFileSystemModel::data(index, Qt::DisplayRole);
    }
    return QFileSystemModel::data(index, role);
}
