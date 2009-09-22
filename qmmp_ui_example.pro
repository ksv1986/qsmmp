SOURCES += main.cpp \
    mainwindow.cpp \
    abstractplaylistmodel.cpp \
    playlistitemdelegate.cpp \
    settingsdialog.cpp \
    settings.cpp
HEADERS += mainwindow.h \
    abstractplaylistmodel.h \
    playlistitemdelegate.h \
    settingsdialog.h \
    settings.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qmmp_ui_example
LIBS += -lqmmp \
    -lqmmpui
FORMS += mainwindow.ui \
    settingsdialog.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /usr/include/
