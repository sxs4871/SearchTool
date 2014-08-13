#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = 0);
    ~ConnectionDialog();

    void setDefaults();

private:
    Ui::ConnectionDialog *ui;

public slots:
    void connectPressed();
};

#endif // CONNECTIONDIALOG_H
