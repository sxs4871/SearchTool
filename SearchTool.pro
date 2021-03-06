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

CONFIG += C++11
         static

SOURCES += main.cpp\
        mainwindow.cpp \
    controller.cpp \
    userquery.cpp \
    hashdictionary.cpp \
    connectiondialog.cpp \
    avu.cpp \
    queryformatexception.cpp \
    irodsquerymodel.cpp \
    primarywindow.cpp \
    editattributedialog.cpp \
    unitsfixer.cpp \
    attributegenerator.cpp

HEADERS  += mainwindow.h \
    controller.h \
    userquery.h \
    hashdictionary.h \
    connectiondialog.h \
    avu.h \
    queryformatexception.h \
    irodsquerymodel.h \
    primarywindow.h \
    editattributedialog.h \
    unitsfixer.h \
    attributegenerator.h

FORMS    += mainwindow.ui \
    connectiondialog.ui \
    editattributedialog.ui

DEFINES += PROJECT_PATH=\"\\\"$$PWD\\\"\"
