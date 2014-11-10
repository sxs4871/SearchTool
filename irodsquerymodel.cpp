#include "irodsquerymodel.h"
#include <controller.h>
#include <userquery.h>
#include <assert.h>

iRODSQueryModel* iRODSQueryModel::instance = NULL;
Controller* iRODSQueryModel::controller = NULL;

bool iRODSQueryModel::connectToDb(QString& hostName, QString& dbName, QString& username, QString& password) {
    disconnect();
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(username);
    db.setPassword(password);
    return db.open();
}

void iRODSQueryModel::disconnect() {
    db.removeDatabase("qt_sql_default_connection");
    db.close();
}

void iRODSQueryModel::runQuery(QSqlQuery & q, QString& queryString) {
    q.exec(queryString);
}

QSqlQuery iRODSQueryModel::runUserQuery(QString userQuery) throw(QueryFormatException){
    UserQuery uq(userQuery);
    QSqlQuery query(db);
    QString queryString = uq.toSQL();
    runQuery(query, queryString);
    return query;
}


QList<AVU> iRODSQueryModel::getFileAttributes(QString fileName) {
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

QSqlError iRODSQueryModel::getLastError() {
    return db.lastError();
}

QString iRODSQueryModel::getDbName() {
    return db.databaseName();
}

void iRODSQueryModel::readAllAttributes() {
    attrNames = new HashDictionary();
    assert (db.isOpen());
    QSqlQuery getAllQuery;
    getAllQuery.exec("SELECT meta_attr_name FROM r_meta_main");
    while (getAllQuery.next()) {
        QString attributeName = getAllQuery.value(0).toString();
        attrNames->insertWord(attributeName);
    }
}

QStringList iRODSQueryModel::findAttributes(QString attrNamePart) {
    return instance->attrNames->findWords(attrNamePart);
}



