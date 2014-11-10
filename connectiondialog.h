#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

class View;

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
    explicit ConnectionDialog(View* _parentView, QWidget *parent = 0);
    ~ConnectionDialog();

    /**
     * @brief setDefaults - presets the input boxes with most likely
     *                      connection information.
     */
    void setDefaults();

    /**
     * @brief getConnectionInfo - grabs data, entered by the user, from
     *                            the input boxes.
     * @return - list of connection information pieces.
     *           1. Host name
     *           2. Database name
     *           3. Username
     *           4. User password
     */
    QStringList getConnectionInfo();

    /**
     * @brief setConnectionError - displays an error, in case connection
     * to the database was not successful.
     * @param errorText - error description
     */
    void setConnectionError(QString errorText);

private:
    /* Qt UI reference for this dialog (created in Forms/connectiondialog.ui */
    Ui::ConnectionDialog *ui;

    /* Reference to the View object to let it know, when this dialog can be closed */
    View* parentView;
};

#endif // CONNECTIONDIALOG_H
