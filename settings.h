#ifndef SETTINGS_H
#define SETTINGS_H

class QString;

class Settings
{
    QString _rootFSCollectionDirectory;
public:
    Settings();

    void setRootFSCollectionDirectory(QString directory);
    const QString& rootFSCollectionDirectory();
};

#endif // SETTINGS_H
