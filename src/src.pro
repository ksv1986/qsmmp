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
    simplefactory.cpp
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
    simplefactory.h
TEMPLATE = lib
CONFIG += warn_on \
    thread \
    qt \
    qxt
QXT += core gui
TARGET = qsmmp
LIBS += -lqmmp \
    -lqmmpui
FORMS += mainwindow.ui \
    popupsettings.ui \
    eqdialog.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /usr/include/
TRANSLATIONS = translations/qsmmp_ru.ts
QMAKE_CXXFLAGS += \
    -Wall -Werror -Wextra \

unix{
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Ui
INSTALLS += target
}

documentation.path = /usr/share/doc/qsmmp
documentation.files = ../README ../COPYING
INSTALLS += documentation
