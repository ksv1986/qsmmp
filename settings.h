#ifndef SETTINGS_H
#define SETTINGS_H

class QString;
class QStringList;

class Settings
{
    QString _rootFSCollectionDirectory;
    bool _startHidden;
    bool _hideOnClose;
    bool _useStandardIcons;
    QStringList _playlistVisibleColumns;

    Settings();
    Settings& operator=(Settings&) {
        return *this;
    }
    Settings(Settings&) {}

    void load();
public:
    static Settings& instance();
    virtual ~Settings();

    void setRootFSCollectionDirectory(QString directory);
    const QString& rootFSCollectionDirectory();

    void setStartHidden(bool startHidden) {
        _startHidden = startHidden;
    }
    bool startHidden() {
        return _startHidden;
    }

    void setHideOnClose(bool hideOnClose) {
        _hideOnClose = hideOnClose;
    }
    bool hideOnClose() {
        return _hideOnClose;
    }

    void setUseStandardIcons(bool useStandardIcons){
        _useStandardIcons = useStandardIcons;
    }
    bool useStandardIcons(){
        return _useStandardIcons;
    }

    void setPlaylistVisibleColumns(QStringList columns);
    QStringList playlistVisibleColumns() {
        return _playlistVisibleColumns;
    }
};

#endif // SETTINGS_H
