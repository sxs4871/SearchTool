#ifndef MODEL_H
#define MODEL_H

#include <qhash.h>
#include <QtSql>
#include <hashdictionary.h>
#include <ui_mainwindow.h>

class Model : public QObject
{

    Q_OBJECT

public:
    static Model* getInstance() {
        if (!instance) {
            instance = new Model();
        }
        return instance;
    }


    bool connectToDb(QString& hostName, QString& dbName, QString& username, QString& password);
    void disconnect();
    void runQuery(QSqlQuery& q, QString& queryString);
    void readAllAttributes();

    void findAttributes(QString attrNamePart);
    void runUserQuery(QString userQuery);

    QSqlError getLastError();
    QString getDbName();

signals:

    void searchCompleted(QSqlQuery);
    void foundAttributes(QStringList);

private:
    /* Singleton constructors, instance */
    Model(){}
    Model(Model const&){}
    Model& operator=(Model const&){}
    static Model* instance;

    QSqlDatabase db;

    HashDictionary* attrNames;
};

#endif // MODEL_H
