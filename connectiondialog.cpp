#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include "primarywindow.h"

ConnectionDialog::ConnectionDialog(PrimaryWindow* _parentView, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
    parentView = _parentView;
    this->setWindowTitle("Enter connection information");
    ui->label_error->setWordWrap(true);
    ui->label_error->setFixedWidth(ui->button_connect->width());
    ui->lineEdit_host->setPlaceholderText("Host address...");
    ui->lineEdit_dbName->setPlaceholderText("Database name...");
    ui->lineEdit_username->setPlaceholderText("Username...");
    ui->lineEdit_password->setPlaceholderText("Password...");
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    QObject::connect(ui->button_connect, SIGNAL(clicked()), parentView, SLOT(dialogConnectPressed()));
    QObject::connect(ui->button_exit, SIGNAL(clicked()), parentView, SLOT(dialogExitPressed()));
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

void ConnectionDialog::setDefaults() {
    ui->lineEdit_host->setText("localhost");
    ui->lineEdit_dbName->setText("ICAT");
    ui->lineEdit_username->setText("irods");
    ui->lineEdit_password->setText("irods");
}



QStringList ConnectionDialog::getConnectionInfo() {
    QStringList toReturn;
    toReturn.push_back(ui->lineEdit_host->text()); 
    toReturn.push_back(ui->lineEdit_dbName->text());
    toReturn.push_back(ui->lineEdit_username->text());
    toReturn.push_back(ui->lineEdit_password->text());
    return toReturn;
}

void ConnectionDialog::setConnectionError(QString errorText) {
    ui->label_error->setText("<font color=\"red\">Database error:</font><p></p>" + errorText);
    ui->label_error->adjustSize();
    this->setFixedHeight(ui->label_error->y() + ui->label_error->height() + 10);
}

