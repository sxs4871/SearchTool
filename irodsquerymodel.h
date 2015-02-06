#ifndef MODEL_H
#define MODEL_H

#include <qhash.h>
#include <QtSql>
#include <hashdictionary.h>
#include <avu.h>
#include <queryformatexception.h>

class Controller;

/**
 * @brief The Model class
 * As prescribed by classical MVC architercture, model holds
 * state of the application and is responsible for database
 * communication. Since all data is stored in iRODS, primary
 * role of this model is retrieving data from the database
 * and submitting user's queries.
 */
class iRODSQueryModel : public QObject
{

    Q_OBJECT

public:
    /**
     * @brief getInstance
     * Model implements Singleton design pattern. Only one state of application can exist.
     * @return Singleton model
     */
    static iRODSQueryModel* getInstance() {
        if (!instance) {
            instance = new iRODSQueryModel();
        }
        return instance;
    }

    /**
     * @brief setController
     * Assign a controller to this model to handle state changes
     * @param _c - assigned controller
     */
    static void setController(Controller* _c) {
        controller = _c;
    }

    /**
     * @brief connectToDb
     * Attempts to connect to a database with the given credentials.
     * @param sqlDriver - name of the Sql driver to use
     * @param hostName - host name
     * @param dbName - database name
     * @param username - username
     * @param password - user's password
     * @return true, if connection was successful
     *         false, otherwise
     */
    bool connectToDb(QString& sqlDriver, QString& hostName, QString& dbName, QString& username, QString& password);

    /**
     * @brief disconnect
     * Closes connection with a database
     */
    void disconnect();

    /**
     * @brief readAllAttributes
     * Reads all the attributes existsing in the r_meta table
     * and stores their name in the 'dictionary' (trie data structure)
     */
    void readAllAttributes();

    void addNewAttribute(int dataId, QString &name, QString &value, QString &units);

    void editAttribute(int metaId, QString &name, QString &value, QString &units);

    void clearAttributes();

    void generateAttributes(int amount);

    /**
     * @brief getFileAttributes
     * Obtains existing metadata for the file
     * with the given name
     * @param fileName - name of the file, for which metadata is requested
     * @return - list of AVUs for the specified file
     */
    QList<AVU> getFileAttributes(int fileId);

    /**
     * @brief findAttributes
     * Finds all attribute names, which begin with the provided string
     * @param attrNamePart - beginning of the attribute name
     * @return - list of matching attribute names
     */
    QStringList findAttributes(QString attrNamePart);

    /**
     * @brief runUserQuery
     * Accepts a 'user's query' (custom format used for this tool),
     * translates this query into SQL and runs it through the database
     * @param userQuery - text of the 'user's query'in 'name = value [unit]' format
     * @return - QSqlQuery object,containing results of the query
     */
    QSqlQuery runUserQuery(QString userQuery) throw(QueryFormatException);

    /**
     * @brief runSqlQuery
     * Runs a SQL query to a database, using a QSqlQuery object,
     * passed by reference. This object will contain results
     * when query is completed.
     * @param q - QSqlQuery object
     * @param queryString - SQL Query text
     */
    QSqlQuery runSqlQuery(QString& queryString);

    /**
     * @brief getLastError
     * Gets the last error reported by the database
     * @return - QSqlError object, containing info about the last error that occured.
     */
    QSqlError getLastError();

    /**
     * @brief getDbName - Accessor for the name of the database
     * @return - name of the database
     */
    QString getDbName();

private:
    /* Constructors and assignment operator hidden to maintain
     * the Singleton pattern integrity */
    iRODSQueryModel() : QObject() {}
    iRODSQueryModel(iRODSQueryModel const&) : QObject() {}
    iRODSQueryModel& operator=(iRODSQueryModel const&) {
        return *this;
    }

    /* Instance variable of the singleton model */
    static iRODSQueryModel* instance;

    /* Pointer to the associated controller */
    static Controller* controller;

    /* Database object. Used to connect to databases */
    QSqlDatabase db;

    /* Dictionary of known attribute names */
    HashDictionary* attrNames;
};

#endif // MODEL_H
