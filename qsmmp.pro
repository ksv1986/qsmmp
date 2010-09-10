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
