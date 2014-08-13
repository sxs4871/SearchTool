#include "model.h"
#include <userquery.h>
#include <assert.h>

Model* Model::instance = NULL;

bool Model::connectToDb(QString& hostName, QString& dbName, QString& username, QString& password) {
    disconnect();
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(username);
    db.setPassword(password);
    return db.open();
}

void Model::disconnect() {
    db.removeDatabase("qt_sql_default_connection");
    db.close();
}

void Model::runQuery(QSqlQuery & q, QString& queryString) {
    q.exec(queryString);
}

QSqlError Model::getLastError() {
    return db.lastError();
}

QString Model::getDbName() {
    return db.databaseName();
}

void Model::readAllAttributes() {
    attrNames = new HashDictionary();
    assert (db.isOpen());
    QSqlQuery getAllQuery;
    getAllQuery.exec("SELECT meta_attr_name FROM r_meta_main");
    while (getAllQuery.next()) {
        QString attributeName = getAllQuery.value(0).toString();
        attrNames->insertWord(attributeName);
    }
}

void Model::findAttributes(QString attrNamePart) {
    QStringList result = instance->attrNames->findWords(attrNamePart);
    emit foundAttributes(result);
}

void Model::runUserQuery(QString userQuery) {
    UserQuery uq(userQuery);
    QSqlQuery query;
    QString queryString = uq.toSQL();
    qDebug() << queryString;
    runQuery(query, queryString);
    emit searchCompleted(query);
}

