#ifndef SETTINGS_H
#define SETTINGS_H

#include <QRect>

class QString;
class QStringList;

class Settings
{
    Q_DISABLE_COPY(Settings)

    QString _rootFSCollectionDirectory;
    bool _startHidden;
    bool _hideOnClose;
    bool _useStandardIcons;
    QSize _size;
    QPoint _pos;
    QByteArray _playlistState;

    Settings();

    void load();
public:
    static Settings& instance();
    virtual ~Settings();

    void reload() {
        load();
    }

    void setRootFSCollectionDirectory(QString directory);
    const QString& rootFSCollectionDirectory();

    void setStartHidden(bool startHidden) {
        _startHidden = startHidden;
    }
    bool startHidden() const {
        return _startHidden;
    }

    void setHideOnClose(bool hideOnClose) {
        _hideOnClose = hideOnClose;
    }
    bool hideOnClose() const {
        return _hideOnClose;
    }

    void setUseStandardIcons(bool useStandardIcons){
        _useStandardIcons = useStandardIcons;
    }
    bool useStandardIcons() const {
        return _useStandardIcons;
    }

    QRect windowGeometry() const {
        return QRect(_pos, _size);
    }
    void setWindowGeometry(QRect val) {
        _pos = val.topLeft();
        _size = val.size();
    }

    QByteArray playlistState() const {
        return _playlistState;
    }
    void setPlaylistState(QByteArray val) {
        _playlistState = val;
    }
};

#endif // SETTINGS_H
