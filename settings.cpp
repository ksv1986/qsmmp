#include <QString>
#include <QSettings>
#include <QDebug>

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
    QSettings settings("qsmmp", "qsmmp");
    settings.beginGroup("mainwindow");
    settings.setValue("hidden", _hidden);
    settings.endGroup();
    settings.beginGroup("player");
    settings.setValue("fscollection", _rootFSCollectionDirectory);
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

void Settings::load()
{
    QSettings settings("qsmmp", "qsmmp");
    settings.beginGroup("mainwindow");
    _hidden = settings.value("hidden", false).toBool();
    settings.endGroup();
    settings.beginGroup("player");
    _rootFSCollectionDirectory = settings.value("fscollection", "/mnt/data/music/").toString();
    settings.endGroup();
}

void Settings::setHidden(bool hidden)
{
    _hidden = hidden;
}
