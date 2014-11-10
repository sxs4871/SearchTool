#ifndef VIEW_H
#define VIEW_H

#include "ui_mainwindow.h"
#include <userquery.h>
#include <QSqlQuery>
#include <QSqlError>
#include "connectiondialog.h"
#include <avu.h>

class Controller;

/**
 * @brief The View class
 * This is the main view of the application. Holds all UI elements
 * in the main window (which are created in Forms/mainwindow.ui,
 * and is responsible for user's interaction with those elements
 */
class PrimaryWindow : public QMainWindow
{
    Q_OBJECT

public:
    /* Constructor and destructor */
    PrimaryWindow(Ui::MainWindow* _ui = 0);
    ~PrimaryWindow();

    /**
     * @brief getUI - Accessor to the main window
     * @return - main window of the application
     */
    Ui::MainWindow* getUI();

    /**
     * @brief setController - associate this view with the given controller
     * @param c - controller to set
     */
    void setController(Controller* c);

    /**
     * @brief getController - Accessor
     * @return - associated controller
     */
    Controller* getController();

    /**
     * @brief askForNewConnection
     * Creates a connection dialog window, asking user to provide information
     * for connection to the database
     */
    void askForNewConnection();

    /**
     * @brief updateSearchResults
     * Displays results of the query in the table of results
     * @param resultQuery - QSqlQuery object with results of the query
     */
    void updateSearchResults(QSqlQuery resultQuery);

    /**
     * @brief showQueryFormatErrors
     * If query entered by user in the local format contained errors,
     * this method displays what those errors were.
     * @param formatErrors - list of errors in the query format
     */
    void showQueryFormatErrors(QStringList formatErrors);

    /**
     * @brief setSearchResultErrorText - diplays error from attempting to run
     * user's query.
     * @param header - hearder of the message
     * @param error - error message(s) itself
     * @param color - color of the header.
     */
    void setSearchResultErrorText(QString header, QString error, QString color);

    /**
     * @brief updateFoundAttributes - lists attributes that were found matching
     * user's request.
     * @param attrNames - names of attributes to display
     */
    void updateFoundAttributes(QStringList attrNames);

    /**
     * @brief showFileAttributes - displays in a table what meta data
     * specific file has.
     * @param avus - all the AVU triplets associated with the file
     * @param fileName - name of the file
     */
    void showFileAttributes(QList<AVU> avus, QString fileName);

    /**
     * @brief displayConnection - sets a label at the bottom of the screen
     * indiciating to which database application is currently connected.
     * @param dbName - name of the database
     */
    void displayConnection(QString dbName);

    /**
     * @brief closeConnection - closes connection with the database,
     * clears labels and offers user to enter new connection info.
     */
    void closeConnection();

    /**
     * @brief closeDialog - removes the connection dialog
     */
    void closeDialog();

    /**
     * @brief setDialogConnectionError - displays an error in the dialog
     * window that is above this window. Error occurs if connection to the
     * database was unsuccessful.
     * @param errorText - description of the error
     */
    void setDialogConnectionError(QString errorText);

    /**
     * @brief quit - terminates application.
     */
    void quit();


private:
    /* Pointer to the maind window of the applicaiton */
    Ui::MainWindow* ui;

    /* Pointer to the associated controller */
    Controller* controller;

    /* Connection dialog, presented to user to give connection credentials */
    ConnectionDialog* cd;

    /**
     * @brief initView
     * Initializes the view. Formats fields, tables, texts, etc.
     * Dloes UI's "make up" :)
     */
    void initView();

public slots:
    /* Slots are methods that are invoked when some particular event occurs.
     * Names of slots describe events that trigger them */
    void searchButtonClicked();
    void attributeSearchUpdated(QString);
    void disconnectButtonPressed();
    void dialogConnectPressed();
    void dialogExitPressed();
    void firstTimeDialog();
    void foundListDoubleClicked(QModelIndex);
    void resultTableRowDoubleClicked(QModelIndex);
};

#endif // VIEW_H
