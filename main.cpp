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

    w.show();

    /* TEST SECTION */
    QRegularExpression ex("\\[.*?\\]");
    QString str("[abc] and [def]");
    QStringList res = ex.match(str).capturedTexts();

    QRegExp rx("\\[.*?\\]");
    rx.setMinimal(true);

    rx.capturedTexts();

    QTimer::singleShot(1, view, SLOT(firstTimeDialog()));

    return a.exec();
}
