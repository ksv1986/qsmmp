#ifndef SETTINGS_H
#define SETTINGS_H

class QString;

class Settings
{
    QString _rootFSCollectionDirectory;
    bool _startHidden;
    bool _hideOnClose;

    Settings();
    Settings& operator=(Settings&) { return *this; }
    Settings(Settings&) {}

    void load();
public:
    static Settings& instance();
    virtual ~Settings();

    void setRootFSCollectionDirectory(QString directory);
    const QString& rootFSCollectionDirectory();

    void setStartHidden(bool startHidden) { _startHidden = startHidden; };
    bool startHidden() { return _startHidden; }

    void setHideOnClose(bool hideOnClose) { _hideOnClose = hideOnClose; };
    bool hideOnClose() { return _hideOnClose; }
};

#endif // SETTINGS_H
