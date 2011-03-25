#ifndef EXTENDEDFILESYSTEMMODEL_H
#define EXTENDEDFILESYSTEMMODEL_H

#include <QFileSystemModel>

class ExtendedFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit ExtendedFileSystemModel(QObject *parent = 0);
    virtual QVariant data(const QModelIndex &index, int role) const;
};

#endif // EXTENDEDFILESYSTEMMODEL_H
