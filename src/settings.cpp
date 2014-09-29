#include <QString>
#include <QSettings>
#include <QDebug>
#include <QStringList>
#include <QPixmap>

#include <qmmp/qmmp.h>

#include "settings.h"

Settings& Settings::instance()
{
    static Settings settings;
    return settings;
}

Settings::Settings()
{
    load();
}

Settings::~Settings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("start_hidden", _startHidden);
    settings.setValue("hide_on_close", _hideOnClose);
    settings.endGroup();
    settings.beginGroup("qsmmp");
    settings.setValue("fscollection", _rootFSCollectionDirectory);
    settings.setValue("use_standard_icons", _useStandardIcons);
    settings.setValue("size", _size);
    settings.setValue("pos", _pos);
    settings.setValue("playlist_state", _playlistState);
    settings.setValue("no_cover_image", _noCoverImage);
    settings.endGroup();
}

const QString& Settings::rootFSCollectionDirectory()
{
    return _rootFSCollectionDirectory;
}

void Settings::setRootFSCollectionDirectory(QString directory)
{
    _rootFSCollectionDirectory = directory;
}

QPixmap Settings::noCoverPixmap() const
{
    return QPixmap(_noCoverImage);
}

void Settings::setNoCoverImage(const QString &path)
{
    _noCoverImage = path;
}

void Settings::load()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    _startHidden = settings.value("start_hidden", false).toBool();
    _hideOnClose = settings.value("hide_on_close", false).toBool();
    settings.endGroup();

    settings.beginGroup("qsmmp");
    _rootFSCollectionDirectory = settings.value("fscollection", "/").toString();
    _useStandardIcons = settings.value("use_standard_icons", true).toBool();
    _size = settings.value("size", QSize(640, 480)).toSize();
    _pos = settings.value("pos", QPoint(0, 0)).toPoint();
    _playlistState = settings.value("playlist_state", QByteArray("")).toByteArray();
    _noCoverImage = settings.value("no_cover_image").toString();
    settings.endGroup();
}
