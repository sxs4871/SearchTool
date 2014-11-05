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
//    QRegularExpression operators(" [=><] | >= | <= ");
//    QString s("a >= 5");
//    qDebug() << operators.match(s).hasMatch();
//    qDebug() << operators.match(s).capturedTexts();

    try {
        UserQuery q("a >= 5");
        QString s("distance = 10 [miles]");
        q.transformSimpleExpr(s);
    } catch (QueryFormatException e) {
        qDebug() << e.getErrorMessage();
    }

    /***************/


    QTimer::singleShot(1, view, SLOT(firstTimeDialog()));

    return a.exec();
}

