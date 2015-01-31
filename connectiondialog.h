#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

class PrimaryWindow;

namespace Ui {
class ConnectionDialog;
}

/**
 * @brief The ConnectionDialog class - This a view class
 * for creating a dialog, which will require user to provide
 * database connection information. This dialog takes over the
 * parent view until successful connection is established.
 */
class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief ConnectionDialog - Constructor requires a reference
     * to the View view that created this dialog to report user's actions
     * to it, when user attempts to connect to the database.
     * @param _parentView - Main View window, which user is going to use,
     *                      when he connects.
     * @param parent - parent widget required by QtDialog constructor
     *                 for QDialog creation
     */
    explicit ConnectionDialog(PrimaryWindow* _parentView, QWidget *parent = 0);
    ~ConnectionDialog();

    /**
     * @brief setDefaults - presets the input boxes with most likely
     *                      connection information.
     */
    void setDefaults();

    /**
     * @brief getSqlDriver - obtain user input for which sql driver
     *                       should be used for connection to the database
     * @return - sql driver name
     */
    QString getSqlDriver();

    /**
     * @brief getHost - get connection info from user
     * @return - host name for connection
     */
    QString getHost();

    /**
     * @brief getDbName - get connection info from user
     * @return - database name for connection
     */
    QString getDbName();

    /**
     * @brief getUsername - get connection info from user
     * @return - username for connection
     */
    QString getUsername();

    /**
     * @brief getPassword - get connection info from user
     * @return - password for connection
     */
    QString getPassword();

    /**
     * @brief setConnectionError - displays an error, in case connection
     * to the database was not successful.
     * @param errorText - error description
     */
    void setConnectionError(QString errorText);

private:
    /* Qt UI reference for this dialog (created in Forms/connectiondialog.ui) */
    Ui::ConnectionDialog *ui;

    /* Reference to the View object to let it know, when this dialog can be closed */
    PrimaryWindow* parentView;
};

#endif // CONNECTIONDIALOG_H
