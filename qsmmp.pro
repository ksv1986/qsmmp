SOURCES += main.cpp \
    mainwindow.cpp \
    abstractplaylistmodel.cpp \
    settings.cpp \
    mytableview.cpp \
    mytreeview.cpp \
    configdialog.cpp \
    pluginitem.cpp \
    volumetoolbutton.cpp
HEADERS += mainwindow.h \
    abstractplaylistmodel.h \
    settings.h \
    mytreeview.h \
    mytableview.h \
    configdialog.h \
    pluginitem.h \
    volumetoolbutton.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qsmmp
LIBS += -lqmmp \
    -lqmmpui
FORMS += mainwindow.ui \
    configdialog.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /usr/include/
