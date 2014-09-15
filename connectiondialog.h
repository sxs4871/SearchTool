#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

class View;

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(View* _parentView, QWidget *parent = 0);
    ~ConnectionDialog();

    void setDefaults();
    QStringList getConnectionInfo();
    void setConnectionError(QString errorText);

private:
    Ui::ConnectionDialog *ui;
    View* parentView;
};

#endif // CONNECTIONDIALOG_H
