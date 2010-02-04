SOURCES += main.cpp \
    mainwindow.cpp \
    abstractplaylistmodel.cpp \
    settingsdialog.cpp \
    settings.cpp \
    mytableview.cpp \
    mytreeview.cpp
HEADERS += mainwindow.h \
    abstractplaylistmodel.h \
    settingsdialog.h \
    settings.h \
    mytreeview.h \
    mytableview.h
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
