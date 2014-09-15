#include "controller.h"
#include "view.h"
#include "model.h"

Controller::Controller(View* _view) {
    model = Model::getInstance();
    view = _view;
    view->setController(this);
    Model::setController(this);
}

bool Controller::connectModelToDb(QString host, QString dbName, QString username, QString password) {
    bool connected = model->connectToDb(host, dbName, username, password);
    if (connected) {
        model->readAllAttributes();
    }
    return connected;
}

void Controller::searchButtonClicked(QString userQuery) {
    QSqlQuery result = model->runUserQuery(userQuery);
    view->updateSearchResults(result);
}

void Controller::attributeSearchUpdated(QString userQuery) {
    QStringList attributes = model->findAttributes(userQuery);
    attributes.sort();
    view->updateFoundAttributes(attributes);
}

void Controller::disconnectButtonPressed() {
    model->disconnect();
    view->closeConnection();
    view->askForNewConnection();
}

void Controller::connectButtonPressed(QStringList connectionParams) {
    QString host = connectionParams[0];
    QString dbName = connectionParams[1];
    QString username = connectionParams[2];
    QString password = connectionParams[3];
    bool connected = connectModelToDb(host, dbName, username, password);
    if (connected) {
        view->closeDialog();
        view->displayConnection(dbName);
    } else {
        QString errorText = model->getLastError().databaseText();
        view->setDialogConnectionError(errorText);
    }
}

void Controller::exitButtonPressed() {
    view->quit();
}

void Controller::resultDoubleClicked(QString fileName) {
    QList<AVU> avus = model->getFileAttributes(fileName);
    view->showFileAttributes(avus, fileName);
}


