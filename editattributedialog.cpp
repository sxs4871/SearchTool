#include "editattributedialog.h"
#include "ui_editattributedialog.h"
#include "primarywindow.h"
#include "qdebug.h"

EditAttributeDialog::EditAttributeDialog(PrimaryWindow* _parentView, QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditAttributeDialog)
{
    ui->setupUi(this);
    parentView = _parentView;
    this->setWindowTitle("Add metadata to \"" + fileName + "\"");

    ui->lineEdit_name->setPlaceholderText("Attribute name...");
    ui->lineEdit_value->setPlaceholderText("Attribute value...");
    ui->lineEdit_unit->setPlaceholderText("Attribute units...(optional)");
    editId = -1;

    QObject::connect(ui->button_accept, SIGNAL(clicked()), parentView, SLOT(editAttrDialogAcceptPressed()));
    QObject::connect(ui->button_cancel, SIGNAL(clicked()), parentView, SLOT(editAttrDialogCancelPressed()));
    QObject::connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(nameEntryChanged(QString)));
    QObject::connect(ui->lineEdit_value, SIGNAL(textChanged(QString)), this, SLOT(valueEntryChanged(QString)));
}


EditAttributeDialog::EditAttributeDialog(PrimaryWindow* _parentView, int metaId, QString &name, QString &value, QString &units, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditAttributeDialog)
{
    ui->setupUi(this);
    parentView = _parentView;
    this->setWindowTitle("Add metadata");
    editId = metaId;

    ui->lineEdit_name->setText(name);
    ui->lineEdit_value->setText(value);
    ui->lineEdit_unit->setText(units);

    QObject::connect(ui->button_accept, SIGNAL(clicked()), parentView, SLOT(editAttrDialogAcceptPressed()));
    QObject::connect(ui->button_cancel, SIGNAL(clicked()), parentView, SLOT(editAttrDialogCancelPressed()));
    QObject::connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(nameEntryChanged(QString)));
    QObject::connect(ui->lineEdit_value, SIGNAL(textChanged(QString)), this, SLOT(valueEntryChanged(QString)));
}

EditAttributeDialog::~EditAttributeDialog()
{
    delete ui;
}

QString EditAttributeDialog::getName() {
    return ui->lineEdit_name->text();
}

QString EditAttributeDialog::getValue() {
    return ui->lineEdit_value->text();
}

QString EditAttributeDialog::getUnits() {
    return ui->lineEdit_unit->text();
}

int EditAttributeDialog::getMetaId() {
    return editId;
}

void EditAttributeDialog::nameEntryChanged(QString name) {
    QRegularExpression whitespace("^\\s*$");
    if (whitespace.match(name).hasMatch()) {
        ui->label_name->setStyleSheet("QLabel { color: red }");
    } else {
        ui->label_name->setStyleSheet("QLabel { color: black }");
    }

}

void EditAttributeDialog::valueEntryChanged(QString value) {
    QRegularExpression whitespace("^\\s*$");
    if (whitespace.match(value).hasMatch()) {
        ui->label_value->setStyleSheet("QLabel { color: red }");
    } else {
        ui->label_value->setStyleSheet("QLabel { color: black }");
    }
}
