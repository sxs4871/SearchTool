#include "mainwindow.h"
#include <QApplication>

#include <model.h>
#include <view.h>
#include <QDebug>
#include <QtSql>
#include <iostream>
#include <userquery.h>
#include <hashdictionary.h>
#include <connectiondialog.h>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Search tool");

    Controller* c = new Controller();
    View* view = new View(w.ui, c);
    w.ui = view->getUI();

    w.show();


    /* TEST SECTION */

    return a.exec();
}

