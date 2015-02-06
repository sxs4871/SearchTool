#include "controller.h"
#include "primarywindow.h"
#include "irodsquerymodel.h"

Controller::Controller() {
    model = iRODSQueryModel::getInstance();
}

void Controller::connectToView(PrimaryWindow* _view) {
    view = _view;
    view->setController(this);
}


bool Controller::connectModelToDb(QString& sqlDriver, QString& host, QString& dbName, QString& username, QString& password) {
    bool connected = model->connectToDb(sqlDriver, host, dbName, username, password);
    if (connected) {
        model->readAllAttributes();
    }
    return connected;
}

void Controller::searchButtonClicked(QString userQuery) {
    try {
        QSqlQuery result = model->runUserQuery(userQuery);
        view->updateSearchResults(result);
    } catch (QueryFormatException e) {
        view->showQueryFormatErrors(e.getErrorMessages());
    }
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

void Controller::cdConnectButtonPressed(QString& sqlDriver, QString& host, QString& dbName, QString& username, QString& password) {
    bool connected = connectModelToDb(sqlDriver, host, dbName, username, password);
    if (connected) {
        view->closeConnectionDialog();
        view->displayConnection(dbName);
    } else {
        QString errorText = model->getLastError().databaseText();
        view->setDialogConnectionError(errorText);
    }
}

void Controller::cdExitButtonPressed() {
    view->quit();
}

void Controller::eadAcceptButtonPressed(int metaId, QString &name, QString &value, QString &units) {
    QRegularExpression whitespace("^\\s*$");
    if (! (whitespace.match(name).hasMatch() && whitespace.match(value).hasMatch())) {
        if (metaId == -1) {
            int dataId = view->getShownAttrOwnerId();
            model->addNewAttribute(dataId, name, value, units);
        } else {
            model->editAttribute(metaId, name, value, units);
        }
        attributeEditCompleted();
        view->closeEditAttrDialog();
    }
}

void Controller::attributeEditCompleted() {
    model->readAllAttributes();
    view->refreshFoundAttributes();
    resultDoubleClicked(view->getShownAttrOwnerId(), view->getShownAttrOwnerName());
}

void Controller::eadCancelButtonPressed() {
    view->closeEditAttrDialog();
}

void Controller::resultDoubleClicked(int fileId, QString fileName) {
    QList<AVU> avus = model->getFileAttributes(fileId);
    view->showFileAttributes(avus, fileName, fileId);
}

void Controller::removeAttributes(QList<int> idsToRemove) {
    foreach (int id, idsToRemove) {
        QString removeMetadata = QString("DELETE FROM r_meta_main WHERE meta_id=\'%1\'").arg(id);
        QString removeMetaMapQuery = QString("DELETE FROM r_objt_metamap WHERE meta_id=\'%1\'").arg(id);
        QSqlQuery first = model->runSqlQuery(removeMetaMapQuery);
        bool firstOk = first.isActive();
        if (!firstOk) {
            qDebug() << "Error: " + first.lastError().text();
        }
        QSqlQuery second = model->runSqlQuery(removeMetadata);
        bool secondOk = second.isActive();
        if (!secondOk) {
            qDebug() << "Error: " + second.lastError().text();
        }
        if (firstOk && secondOk) {
            attributeEditCompleted();
        }
    }
}

void Controller::regenerateAttributes(int avgPerFile) {
    model->clearAttributes();
    model->generateAttributes(avgPerFile);
    attributeEditCompleted();
}

