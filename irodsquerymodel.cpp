#include "irodsquerymodel.h"
#include <controller.h>
#include <userquery.h>
#include <assert.h>

iRODSQueryModel* iRODSQueryModel::instance = NULL;
Controller* iRODSQueryModel::controller = NULL;

bool iRODSQueryModel::connectToDb(QString& sqlDriver, QString& hostName, QString& dbName, QString& username, QString& password) {
    disconnect();

    db = QSqlDatabase::addDatabase(sqlDriver);
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(username);
    db.setPassword(password);
//    return db.open();

    QStringList sqlDrivers = QSqlDatabase::drivers();
    qDebug() << "Available drivers: " << sqlDrivers;
    foreach (QString driver, sqlDrivers) {
        qDebug() << "Trying " << driver;
        db = QSqlDatabase::addDatabase(driver, driver);
        if (db.driver()->lastError().type() == 0 && driver != "QSQLITE") {
            db.setHostName(hostName);
            db.setDatabaseName(dbName);
            db.setUserName(username);
            db.setPassword(password);
            if (db.open()) {
                break;
            }
        }
        QSqlDatabase::removeDatabase(driver);
    }
    return db.isOpen();
}

void iRODSQueryModel::disconnect() {
    db.removeDatabase("qt_sql_default_connection");
    db.close();
}

QSqlQuery iRODSQueryModel::runSqlQuery(QString& queryString) {
    qDebug() << "Running query: " + queryString;
    QSqlQuery query(db);
    query.exec(queryString);
    return query;
}

QSqlQuery iRODSQueryModel::runUserQuery(QString userQuery) throw(QueryFormatException){
    UserQuery uq(userQuery);
    QString queryString = uq.toSQL();
    return runSqlQuery(queryString);;
}

QList<AVU> iRODSQueryModel::getFileAttributes(int fileId) {
    QList<AVU> avus;
    QSqlQuery query(db);
    QString queryText = QString("SELECT DISTINCT m.meta_attr_name, m.meta_attr_value, m.meta_attr_unit, m.meta_id FROM r_data_main d, r_meta_main m, r_objt_metamap om WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and d.data_id=\'%1\'").arg(fileId);
    query.exec(queryText);
    if (query.isActive()) {
        while (query.next()) {
            QString attrName = query.value(0).toString();
            QString value = query.value(1).toString();
            QString unit = query.value(2).toString();
            int id = query.value(3).toInt();
            AVU nextAVU(attrName, value, unit);
            nextAVU.setId(id);
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
    QSqlQuery getAllQuery(db);
    getAllQuery.exec("SELECT meta_attr_name FROM r_meta_main");
    while (getAllQuery.next()) {
        QString attributeName = getAllQuery.value(0).toString();
        attrNames->insertWord(attributeName);
    }
}


void iRODSQueryModel::addNewAttribute(int dataId, QString &name, QString &value, QString &units) {
    QString getMetaIdsQuery("SELECT meta_id FROM r_meta_main");
    QSqlQuery result = runSqlQuery(getMetaIdsQuery);
    int maxId = 0;
    while (result.next()) {
        if (result.value(0).toInt() > maxId) {
            maxId = result.value(0).toInt();
        }
    }
    int newId = maxId + 1;
    long ts = QDateTime::currentDateTime().toTime_t();
    QString timestamp = QString("%1").arg(ts, 11, 10, QChar('0'));
    QString metaMainQuery = QString("INSERT INTO r_meta_main "
                                    "VALUES (%1, NULL, \'%2\', \'%3\', \'%4\', NULL, \'%5\', \'%6\')"
                                    ).arg(QString::number(newId), name, value, units, timestamp, timestamp);
    QString metaMapQuery = QString("INSERT INTO r_objt_metamap "
                                   "VALUES (%1, %2, \'%3\', \'%4\')"
                                   ).arg(QString::number(dataId), QString::number(newId), timestamp, timestamp);
    runSqlQuery(metaMainQuery);
    runSqlQuery(metaMapQuery);
}

void iRODSQueryModel::editAttribute(int metaId, QString &name, QString &value, QString &units) {
    long ts = QDateTime::currentDateTime().toTime_t();
    QString timestamp = QString("%1").arg(ts, 11, 10, QChar('0'));
    QString editQuery = QString("UPDATE r_meta_main "
                                "SET meta_attr_name=\'%2\', meta_attr_value=\'%3\', meta_attr_unit=\'%4\', modify_ts=\'%5\'"
                                "WHERE meta_id=\'%1\' ").arg(QString::number(metaId), name, value, units, timestamp);
    runSqlQuery(editQuery);
}

QStringList iRODSQueryModel::findAttributes(QString attrNamePart) {
    return instance->attrNames->findWords(attrNamePart);
}
