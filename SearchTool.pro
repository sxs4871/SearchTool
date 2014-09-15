#-------------------------------------------------
#
# Project created by QtCreator 2014-07-27T04:27:56
#
#-------------------------------------------------

QT       += core gui\
            sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SearchTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    view.cpp \
    model.cpp \
    controller.cpp \
    userquery.cpp \
    hashdictionary.cpp \
    connectiondialog.cpp \
    avu.cpp

HEADERS  += mainwindow.h \
    view.h \
    model.h \
    controller.h \
    userquery.h \
    hashdictionary.h \
    connectiondialog.h \
    avu.h

FORMS    += mainwindow.ui \
    connectiondialog.ui
