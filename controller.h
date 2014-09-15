#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>

class View;
class Model;

class Controller
{
public:
    Controller(View*);

    void searchButtonClicked(QString userQuery);
    void attributeSearchUpdated(QString);
    void disconnectButtonPressed();
    void connectionAccepted();
    void connectionRejected();
    bool connectModelToDb(QString host, QString dbName, QString username, QString password);
    void connectButtonPressed(QStringList connectionParams);
    void exitButtonPressed();
    void resultDoubleClicked(QString fileName);

private:
    Model* model;
    View* view;
};

#endif // CONTROLLER_H
