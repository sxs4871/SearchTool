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
    ui->comboBox_sqlDrivers->addItem("");
    QStringList sqlDrivers = QSqlDatabase::drivers();
    ui->comboBox_sqlDrivers->addItems(sqlDrivers);

    QObject::connect(ui->button_connect, SIGNAL(clicked()), parentView, SLOT(connectionDialogConnectPressed()));
    QObject::connect(ui->button_exit, SIGNAL(clicked()), parentView, SLOT(connectionDialogExitPressed()));
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

void ConnectionDialog::setConnectionError(QString errorText) {
    ui->label_error->setText("<font color=\"red\">Database error:</font><p></p>" + errorText);
    ui->label_error->adjustSize();
    this->setFixedHeight(ui->label_error->y() + ui->label_error->height() + 10);
}

QString ConnectionDialog::getSqlDriver() {
    return ui->comboBox_sqlDrivers->currentText();
}

QString ConnectionDialog::getHost() {
    return ui->lineEdit_host->text();
}

QString ConnectionDialog::getDbName() {
    return ui->lineEdit_dbName->text();
}

QString ConnectionDialog::getUsername() {
    return ui->lineEdit_username->text();
}

QString ConnectionDialog::getPassword() {
    return ui->lineEdit_password->text();
}
