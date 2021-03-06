QT += widgets
UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj/

SOURCES += \
    mainwindow.cpp \
    abstractplaylistmodel.cpp \
    settings.cpp \
    playlistview.cpp \
    pluginitem.cpp \
    volumetoolbutton.cpp \
    popupsettings.cpp \
    popupwidget.cpp \
    eqdialog.cpp \
    visualmenu.cpp \
    trackslider.cpp \
    extendedfilesystemmodel.cpp \
    simplefactory.cpp \
    settingswidget.cpp \
    playlistswidget.cpp \
    playlistbar.cpp \
    main.cpp
HEADERS += mainwindow.h \
    abstractplaylistmodel.h \
    settings.h \
    playlistview.h \
    pluginitem.h \
    volumetoolbutton.h \
    popupsettings.h \
    popupwidget.h \
    visualmenu.h \
    eqdialog.h \
    trackslider.h \
    extendedfilesystemmodel.h \
    simplefactory.h \
    settingswidget.h \
    playlistswidget.h \
    playlistbar.h
TEMPLATE = lib
CONFIG += warn_on \
    thread \
    qt \
    plugin

TARGET = qsmmp
LIBS += -lqmmp \
    -lqmmpui
FORMS += mainwindow.ui \
    popupsettings.ui \
    eqdialog.ui \
    settingswidget.ui
RESOURCES += stuff.qrc
TRANSLATIONS = translations/qsmmp_ru.ts
QMAKE_CXXFLAGS += \
    -Wall -Werror -Wextra \

unix{
isEmpty(LIB_DIR){
    !macx:contains(QMAKE_HOST.arch, x86_64) {
        LIB_DIR = /usr/lib64
    } else {
        LIB_DIR = /usr/lib
    }
}
target.path = $$LIB_DIR/qmmp/Ui
INSTALLS += target
}

documentation.path = /usr/share/doc/qsmmp
documentation.files = ../README ../COPYING
INSTALLS += documentation
