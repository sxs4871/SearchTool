#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>

/* Forward declarations required to let C++ objects
 * contain pointers to each other */
class View;
class Model;

/**
 * @brief The Controller class
 * Controller in the MVC model. Receives events from a view,
 * with which it is associated and takes proper actions, modifying
 * or quering model and/or making view behave accordingly
 */
class Controller
{
public:
    /**
     * @brief Controller
     * For transparency of code sake, controller is created without a view,
     * however it will need a view to be associated with, so setView will have to
     * be called after. Model is obtained within constructor, because Model is a
     * singleton.
     */
    Controller();

    /**
     * @brief setView - associates this controller with some view.
     * Connection goes both ways (controller --> view and view --> controller)
     * @param _view
     */
    void connectToView(View* _view);

    /* Following methods are created to respond to certain events
       in that happen in the view */

    /**
     * @brief searchButtonClicked - user pressed 'Search'
     * @param userQuery - query entered by the user
     */
    void searchButtonClicked(QString userQuery);

    /**
     * @brief attributeSearchUpdated - text entered in the attribute search field
     * was changed. This means that results need to be updated
     */
    void attributeSearchUpdated(QString);

    /**
     * @brief disconnectButtonPressed
     * User pressed disconnect from the database. Close database connection
     * and present new connection dialog
     */
    void disconnectButtonPressed();

    /**
     * @brief connectionAccepted - connection to the database was successful.
     * Need to close the connection dialog and present the main view to the user.
     */
    void connectionAccepted();

    /**
     * @brief connectionRejected
     * Connection to the database was unsuccessful. Display error and let user try again.
     */
    void connectionRejected();

    /**
     * @brief connectModelToDb
     * Use information provided by the user to try to connect to the database.
     * @param host - host name
     * @param dbName - database name
     * @param username - username
     * @param password - user password
     * @return - true, if connection was successful
     *           false, otherwise
     */
    bool connectModelToDb(QString host, QString dbName, QString username, QString password);

    /**
     * @brief connectButtonPressed - user pressed 'Connect' button in the connection dialog
     * @param connectionParams - list of connection parameters entered by the user
     *                           1. Host name
     *                           2. Database name
     *                           3. Username
     *                           4. User password
     */
    void connectButtonPressed(QStringList connectionParams);

    /**
     * @brief exitButtonPressed - user wants to quit the program. Close everything.
     */
    void exitButtonPressed();

    /**
     * @brief resultDoubleClicked
     * User double-clicked some result in the results table, which means he wants more
     * information about it. Present all the metadata associated with the selected object
     * in a separate table
     * @param fileName - name of the file that user double clicked.
     */
    void resultDoubleClicked(QString fileName);

private:
    /* References to model and view */
    Model* model;
    View* view;
};

#endif // CONTROLLER_H
