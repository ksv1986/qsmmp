#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <qmmp/qmmp.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a (argc, argv );
    QTranslator translator;
    QString locale = Qmmp::systemLanguageID();
    translator.load(QString(":/qmmp_") + locale);
    a.installTranslator(&translator);

    QTranslator qt_translator;
    qt_translator.load(QLibraryInfo::location (QLibraryInfo::TranslationsPath) + "/qt_" + locale);
    a.installTranslator(&qt_translator);

    new MediaPlayer(&a);
    new UiHelper(&a);

    MainWindow window;
    window.show();
    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
