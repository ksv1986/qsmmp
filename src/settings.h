#ifndef SETTINGS_H
#define SETTINGS_H

#include <QRect>

class QString;
class QStringList;
class QPixmap;

class Settings
{
    QString _rootFSCollectionDirectory;
    QString _noCoverImage;
    bool _startHidden;
    bool _hideOnClose;
    bool _useStandardIcons;
    QSize _size;
    QPoint _pos;
    QByteArray _playlistState;

    Settings();
    Settings& operator=(Settings&) {
        return *this;
    }
    Settings(Settings&) {}

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

    QRect windowGeometry() {
        return QRect(_pos, _size);
    }
    void setWindowGeometry(QRect val) {
        _pos = val.topLeft();
        _size = val.size();
    }

    QByteArray playlistState() {
        return _playlistState;
    }
    void setPlaylistState(QByteArray val) {
        _playlistState = val;
    }
    QPixmap noCoverPixmap() const;
    void setNoCoverImage(const QString &path);
};

#endif // SETTINGS_H
