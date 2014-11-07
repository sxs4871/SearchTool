#ifndef MODEL_H
#define MODEL_H

#include <qhash.h>
#include <QtSql>
#include <hashdictionary.h>
#include <avu.h>
#include <queryformatexception.h>

class Controller;

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

    static void setController(Controller* _c) {
        //c = _c;
    }

    bool connectToDb(QString& hostName, QString& dbName, QString& username, QString& password);
    void disconnect();
    void readAllAttributes();
    void runQuery(QSqlQuery& q, QString& queryString);
    QList<AVU> getFileAttributes(QString fileName);

    QStringList findAttributes(QString attrNamePart);
    QSqlQuery runUserQuery(QString userQuery) throw(QueryFormatException);

    QSqlError getLastError();
    QString getDbName();

private:
    /* Singleton constructors, instance */
    Model(){}
    Model(Model const&){}
    Model& operator=(Model const&){}
    static Model* instance;
    static Controller* c;

    QSqlDatabase db;

    HashDictionary* attrNames;
};

#endif // MODEL_H
