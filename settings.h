#ifndef SETTINGS_H
#define SETTINGS_H

class QString;

class Settings
{
    QString _rootFSCollectionDirectory;
    bool _hidden;

    Settings();
    Settings& operator=(Settings&) { return *this; }
    Settings(Settings&) {}

    void load();
public:
    static Settings& instance();
    virtual ~Settings();

    void setRootFSCollectionDirectory(QString directory);
    const QString& rootFSCollectionDirectory();

    void setHidden(bool hidden);
    bool hidden() { return _hidden; }
};

#endif // SETTINGS_H
