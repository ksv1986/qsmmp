SOURCES += main.cpp \
    mainwindow.cpp \
    abstractplaylistmodel.cpp \
    settings.cpp \
    mytableview.cpp \
    mytreeview.cpp \
    pluginitem.cpp \
    volumetoolbutton.cpp \
    configdialog.cpp \
    popupsettings.cpp \
    popupwidget.cpp
HEADERS += mainwindow.h \
    abstractplaylistmodel.h \
    settings.h \
    mytreeview.h \
    mytableview.h \
    pluginitem.h \
    volumetoolbutton.h \
    configdialog.h \
    popupsettings.h \
    popupwidget.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qsmmp
LIBS += -lqmmp \
    -lqmmpui
FORMS += mainwindow.ui \
    configdialog.ui \
    popupsettings.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /usr/include/
