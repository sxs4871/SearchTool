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

//    QRegularExpression e("\\((?>[^()]|(?R))*\\)");
//    QString ss("1 + (((abc)+4) + (9 - 5))");
//    qDebug() << e.match(ss).hasMatch();
//    qDebug() << e.match(ss).capturedTexts();

//    QString nonEscape("(?<!\\\\)");
//    QRegularExpression e("\\s+" + nonEscape + "(AND|OR|\\|{2}|\\&{2})\\s+");
//    QString s("a=5 OR v>7");
//    qDebug() << s.split(e);

    try {
        UserQuery q("3 + (((a > b) && e) AND (c = d)");
        q.toSQL();
    } catch (QueryFormatException e) {
        qDebug() << e.getErrorMessage();
    }

    /***************/


    QTimer::singleShot(1, view, SLOT(firstTimeDialog()));

    return a.exec();
}

