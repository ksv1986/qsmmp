#include <QString>
#include <QSettings>
#include <QDebug>

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
    settings.setValue("_startHidden", _startHidden);
    settings.endGroup();
    settings.beginGroup("qsmmp");
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
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    _startHidden = settings.value("start_hidden", false).toBool();
    _hideOnClose = settings.value("hide_on_close", false).toBool();
    settings.endGroup();

    settings.beginGroup("qsmmp");
    _rootFSCollectionDirectory = settings.value("fscollection", "/").toString();
    settings.endGroup();
}
