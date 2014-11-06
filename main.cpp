#include "mainwindow.h"
#include <QApplication>

#include <model.h>
#include <view.h>
#include <controller.h>
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
    w.setFixedSize(w.width(), w.height());

    View* view = new View(w.ui);
    Controller* c = new Controller(view);
    w.ui = view->getUI();

    //w.show();

    /* TEST SECTION */

    UserQuery q("");
    QString s("AND a AND(b AND c)OR(d AND e)ANDd");
    q.transformQuery(s);


    /***************/


    QTimer::singleShot(1, view, SLOT(firstTimeDialog()));

    return a.exec();
}

