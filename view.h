#ifndef VIEW_H
#define VIEW_H

#include "ui_mainwindow.h"
#include <userquery.h>
#include <QSqlQuery>
#include <QSqlError>
#include "connectiondialog.h"
#include <avu.h>

class Controller;

class View : public QMainWindow
{
    Q_OBJECT

public:
    View(Ui::MainWindow* _ui = 0);
    ~View();
    Ui::MainWindow* getUI();
    void setController(Controller* c);
    Controller* getController();

    void askForNewConnection();
    void showConnectionDialog();

    void updateSearchResults(QSqlQuery);
    void updateFoundAttributes(QStringList);
    void showFileAttributes(QList<AVU>, QString);

    void displayConnection(QString dbName);
    void closeConnection();
    void closeDialog();
    void setDialogConnectionError(QString errorText);
    void quit();


private:
    Ui::MainWindow* ui;
    Controller* controller;
    ConnectionDialog* cd;
    void initView();

public slots:
    void searchButtonClicked();
    void attributeSearchUpdated(QString);
    void disconnectButtonPressed();
    void dialogConnectPressed();
    void dialogExitPressed();
    void firstTimeDialog();
    void foundListDoubleClicked(QModelIndex);
    void resultTableRowDoubleClicked(QModelIndex);
};

#endif // VIEW_H
