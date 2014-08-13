#ifndef VIEW_H
#define VIEW_H

#include "ui_mainwindow.h"
#include <model.h>
#include <userquery.h>
#include <controller.h>

class View : public QMainWindow
{
    Q_OBJECT

public:
    View(Ui::MainWindow* _ui = 0, Controller* _c = 0);
    ~View();
    Ui::MainWindow* getUI();
    void askForNewConnection();

    void updateSearchResults(QSqlQuery query);

private:
    Ui::MainWindow* ui;
    Model* model;
    Controller* controller;
    void initView();

public slots:
    void searchButtonClicked();
    void attributeSearchUpdated(QString);
    void disconnectButtonPressed();
    void modelFinishedSearch(QSqlQuery);
    void modelFoundAttributes(QStringList);
};

#endif // VIEW_H
