UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj/

SOURCES += main.cpp \
    mainwindow.cpp \
    abstractplaylistmodel.cpp \
    settings.cpp \
    playlistview.cpp \
    mytreeview.cpp \
    pluginitem.cpp \
    volumetoolbutton.cpp \
    configdialog.cpp \
    popupsettings.cpp \
    popupwidget.cpp \
    eqdialog.cpp \
    visualmenu.cpp \
    trackslider.cpp
HEADERS += mainwindow.h \
    abstractplaylistmodel.h \
    settings.h \
    mytreeview.h \
    playlistview.h \
    pluginitem.h \
    volumetoolbutton.h \
    configdialog.h \
    popupsettings.h \
    popupwidget.h \
    visualmenu.h \
    eqdialog.h \
    trackslider.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qsmmp
LIBS += -lqmmp \
    -lqmmpui
FORMS += mainwindow.ui \
    configdialog.ui \
    popupsettings.ui \
    eqdialog.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /usr/include/
TRANSLATIONS = translations/qsmmp_ru.ts
QMAKE_CXXFLAGS += \
    -Wall -Werror -Wextra \

target.path = /usr/bin
documentation.path = /usr/share/doc/qsmmp
documentation.files = ../README ../COPYING
desktop.path = /usr/share/applications
desktop.files = ../qsmmp.desktop
INSTALLS += target documentation desktop
