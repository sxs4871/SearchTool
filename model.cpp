#include "model.h"
#include <controller.h>
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

QSqlQuery Model::runUserQuery(QString userQuery) {
    UserQuery uq(userQuery);
    QSqlQuery query;
    QString queryString = uq.toSQL();
    qDebug() << queryString;
    runQuery(query, queryString);
    return query;
}


QList<AVU> Model::getFileAttributes(QString fileName) {
    QList<AVU> avus;
    QSqlQuery query;
    QString queryText = "SELECT DISTINCT m.meta_attr_name, m.meta_attr_value, m.meta_attr_unit FROM r_data_main d, r_meta_main m, r_objt_metamap om WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and d.data_name=\'" + fileName + "\'";
    query.exec(queryText);
    if (query.isActive()) {
        while (query.next()) {
            QString attrName = query.value(0).toString();
            QString value = query.value(1).toString();
            QString unit = query.value(2).toString();
            AVU nextAVU(attrName, value, unit);
            avus.push_back(nextAVU);
        }
    }
    return avus;
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

QStringList Model::findAttributes(QString attrNamePart) {
    return instance->attrNames->findWords(attrNamePart);
}



