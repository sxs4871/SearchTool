#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include "model.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Enter connection information");
    ui->label_error->setWordWrap(true);
    ui->label_error->setFixedWidth(ui->button_connect->width());
    ui->lineEdit_db_name->setPlaceholderText("Database name...");
    ui->lineEdit_host->setPlaceholderText("Host address...");
    ui->lineEdit_username->setPlaceholderText("Username...");
    ui->lineEdit_password->setPlaceholderText("Password...");
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    QObject::connect(ui->button_connect, SIGNAL(clicked()), this, SLOT(connectPressed()));
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

void ConnectionDialog::setDefaults() {
    ui->lineEdit_db_name->setText("ICAT");
    ui->lineEdit_host->setText("localhost");
    ui->lineEdit_username->setText("irods");
    ui->lineEdit_password->setText("irods");
}

void ConnectionDialog::connectPressed() {
    QString dbName(ui->lineEdit_db_name->text());
    QString host(ui->lineEdit_host->text());
    QString username(ui->lineEdit_username->text());
    QString password(ui->lineEdit_password->text());
    Model* model = Model::getInstance();
    bool connected = model->connectToDb(host, dbName, username, password);
    if (connected) {
        QDialog::accept();
    } else {
        ui->label_error->setText("<font color=\"red\">Database error:</font><p></p>" + model->getLastError().databaseText());
        ui->label_error->adjustSize();
        this->setFixedHeight(ui->label_error->y() + ui->label_error->height() + 10);
    }
}
