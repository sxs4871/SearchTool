#include "primarywindow.h"
#include <controller.h>
#include <QDateTime>
#include <QTableView>
#include <qdebug.h>
#include <QProcess>

PrimaryWindow::PrimaryWindow(Ui::MainWindow* _ui) {
    if (_ui == 0) {
        new Ui::MainWindow();
    } else {
        ui = _ui;
    }
    initView();
}

PrimaryWindow::~PrimaryWindow() {}

void PrimaryWindow::initView() {

    /* GroupBox setup */
    QString groupBoxStyle("QGroupBox {font-size: 14px; font-weight: bold;border: 1px solid gray;border-radius: 5px;margin-top: 0.5em;}\
            QGroupBox::title {subcontrol-origin: margin;left: 10px;padding: 0 3px 0 3px;}");
    ui->group_findAttr->setStyleSheet(groupBoxStyle);
    ui->group_fileAttributes->setStyleSheet(groupBoxStyle);
    ui->group_setValue->setStyleSheet(groupBoxStyle);


    /* LineEdit setup */
    ui->lineEdit_findAttr->setStyleSheet("QLineEdit { border: 1px solid gray; border-radius: 10px; }");
    ui->lineEdit_query->setStyleSheet("QLineEdit { border: 1px solid gray; border-radius: 13px; }");
    ui->lineEdit_findAttr->setPlaceholderText("  Find attribute...");
    ui->lineEdit_query->setPlaceholderText("   Enter your query...");

    /* Table of results setup */
    ui->table_results->setColumnCount(3);
    ui->table_results->setHorizontalHeaderLabels(QStringList() << "File name" << "Collection" << "Date modified");
    ui->table_results->horizontalHeader()->setVisible(true);
    int tableWidth = ui->table_results->width();
    int colCount = ui->table_results->columnCount();
    for (int i = 0; i < colCount; i++) {
        ui->table_results->setColumnWidth(i, tableWidth / 3);
    }
    ui->table_results->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_results->horizontalHeader()->setStretchLastSection(true);
    ui->table_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->label_queryMessage->setHidden(true);
    ui->label_queryMessage->setFixedWidth(ui->table_results->width() - 20);
    ui->label_queryMessage->setWordWrap(true);


    /* Table of attributes setup */
    ui->table_attributes->setColumnCount(3);
    ui->table_attributes->setHorizontalHeaderLabels(QStringList() << "Attribute name" << "Value" << "Unit");
    ui->table_attributes->horizontalHeader()->setVisible(true);
    ui->table_attributes->verticalHeader()->setVisible(false);
    tableWidth = ui->table_attributes->width();
    colCount = ui->table_attributes->columnCount();
    ui->table_attributes->setColumnWidth(0, tableWidth / 7 * 3);
    ui->table_attributes->setColumnWidth(1, tableWidth / 7 * 2);
    ui->table_attributes->setColumnWidth(2, tableWidth / 7 * 2);
    ui->table_attributes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_attributes->horizontalHeader()->setStretchLastSection(true);
    ui->table_attributes->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    /* Lists setup */
    ui->list_foundAttr->setStyleSheet("background-color: transparent;");
}

void PrimaryWindow::setController(Controller* c) {
    controller = c;
    /* connecting signals and slots */
    QObject::connect(ui->lineEdit_query, SIGNAL(returnPressed()), ui->button_search, SIGNAL(clicked()));
    QObject::connect(ui->button_search, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
    QObject::connect(ui->lineEdit_findAttr, SIGNAL(textChanged(QString)), this, SLOT(attributeSearchUpdated(QString)));
    QObject::connect(ui->button_disconnect, SIGNAL(clicked()), this, SLOT(disconnectButtonPressed()));
    QObject::connect(ui->list_foundAttr, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(foundListDoubleClicked(QModelIndex)));
    QObject::connect(ui->table_results, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(resultTableRowDoubleClicked(QModelIndex)));
    QObject::connect(ui->button_newAttr, SIGNAL(clicked()), this, SLOT(newAttributeClicked()));
    QObject::connect(ui->button_editAttr, SIGNAL(clicked()), this, SLOT(editAttributeClicked()));
    QObject::connect(ui->button_removeAttr, SIGNAL(clicked()), this, SLOT(removeAttributeClicked()));
}

Controller* PrimaryWindow::getController() {
    return controller;
}

Ui::MainWindow* PrimaryWindow::getUI() {
    return ui;
}

void PrimaryWindow::askForNewConnection() {
    connectionDialog = new ConnectionDialog(this);
    connectionDialog->setFixedSize(connectionDialog->width(), connectionDialog->height());
    connectionDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    connectionDialog->setDefaults();
    while (connectionDialog->exec() != QDialog::Accepted);
    delete connectionDialog;
    connectionDialog = 0;
}

void PrimaryWindow::editAttribute() {
    QModelIndexList selectedRows = ui->table_attributes->selectionModel()->selectedRows();
    if (selectedRows.size() != 1) {
        return;
    }
    int row = selectedRows.at(0).row();
    QString name = ui->table_attributes->item(row, 0)->text();
    QString value = ui->table_attributes->item(row, 1)->text();
    QString units = ui->table_attributes->item(row, 2)->text();
    int metaId = shownAttrIds.at(row);
    editAttrDialog = new EditAttributeDialog(this, metaId, name, value, units);
    editAttrDialog->setFixedSize(editAttrDialog->width(), editAttrDialog->height());
    editAttrDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    while (editAttrDialog->exec() != QDialog::Accepted);
    delete editAttrDialog;
    editAttrDialog = 0;
}

void PrimaryWindow::addNewAttribute() {
    QString fileName = shownAttrOwnerName;
    editAttrDialog = new EditAttributeDialog(this, fileName);
    editAttrDialog->setFixedSize(editAttrDialog->width(), editAttrDialog->height());
    editAttrDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    while (editAttrDialog->exec() != QDialog::Accepted);
    delete editAttrDialog;
    editAttrDialog = 0;
}

void PrimaryWindow::updateSearchResults(QSqlQuery result) {
    ui->table_results->setRowCount(0);
    bool ok = result.isActive();
    if (!ok) {
        setSearchResultErrorText("Query database error:", result.lastError().databaseText(), "red");
        return;
    }

    int resultCount = result.size();
    if (resultCount == 0) {
        setSearchResultErrorText("No results found", "", "orange");
        return;
    }
    ui->label_queryMessage->setHidden(true);
    shownDataIds.clear();
    int lastRow = 0;
    while (result.next()) {
        ui->table_results->insertRow(ui->table_results->rowCount());
        ui->table_results->setItem(lastRow, 0, new QTableWidgetItem(result.value(0).toString()));
        ui->table_results->setItem(lastRow, 1, new QTableWidgetItem(result.value(1).toString()));
        QString unixTimestamp = result.value(2).toString();
        bool converted;
        uint unixTimestampInteger = unixTimestamp.toUInt(&converted);
        if (converted) {
            QDateTime modificationDate = QDateTime::fromTime_t(unixTimestampInteger);
            ui->table_results->setItem(lastRow, 2, new QTableWidgetItem(modificationDate.toString()));
        } else {
            ui->table_results->setItem(lastRow, 2, new QTableWidgetItem("Invalid timestamp"));
        }
        shownDataIds.append(result.value(3).toInt());
        lastRow++;
    }
}

void PrimaryWindow::showQueryFormatErrors(QStringList formatErrors) {
    QString errorText;
    foreach (QString error, formatErrors) {
        errorText += QString("%1<p></p>").arg(error);
    }
    setSearchResultErrorText("Query contains format errors:", errorText, "red");
}

void PrimaryWindow::setSearchResultErrorText(QString header, QString error, QString color) {
    ui->table_results->setRowCount(0);
    QString coloredText = QString("<font color=\"%1\">%2</font><p></p>%3").arg(color, header, error);
    ui->label_queryMessage->setText(coloredText);
    ui->label_queryMessage->adjustSize();
    ui->label_queryMessage->setHidden(false);
    return;
}

void PrimaryWindow::updateFoundAttributes(QStringList attributes) {
    ui->list_foundAttr->clear();
    ui->list_foundAttr->addItems(attributes);
}

void PrimaryWindow::refreshFoundAttributes() {
    QString currentSearch = ui->lineEdit_findAttr->text();
    controller->attributeSearchUpdated(currentSearch);
}

void PrimaryWindow::showFileAttributes(QList<AVU> avus, QString fileName, int fileId) {
    shownAttrOwnerId = fileId;
    shownAttrOwnerName = fileName;
    QString title = "\"" + fileName + "\" attributes";
    ui->group_fileAttributes->setTitle(title);
    ui->table_attributes->setRowCount(0);
    int tableSize = avus.count();
    shownAttrIds.clear();
    for (int i = 0; i < tableSize; i++) {
        AVU curAVU = avus.at(i);
        ui->table_attributes->insertRow(ui->table_attributes->rowCount());
        ui->table_attributes->setItem(i, 0, new QTableWidgetItem(curAVU.getAttrName()));
        ui->table_attributes->setItem(i, 1, new QTableWidgetItem(curAVU.getValue()));
        ui->table_attributes->setItem(i, 2, new QTableWidgetItem(curAVU.getUnit()));
        shownAttrIds.append(curAVU.getId());
    }
    ui->button_newAttr->setEnabled(true);
    ui->button_editAttr->setEnabled(true);
    ui->button_removeAttr->setEnabled(true);
}

void PrimaryWindow::removeAttributeFromTable(int metaId) {
    int index = shownAttrIds.indexOf(metaId);
    shownAttrIds.removeAt(index);
    ui->table_attributes->removeRow(index);
}

void PrimaryWindow::displayConnection(QString dbName) {
    this->attributeSearchUpdated("");
    QLabel* c = ui->label_connection;
    c->setText("Connected to:  \"" + dbName + "\"");
    c->setStyleSheet("QLabel { color : green; }");
    c->adjustSize();
    c->setFixedHeight(27);
    int x = c->x() + c->width() + 20;
    int width = ui->table_attributes->x() + ui->table_attributes->width() - x;
    ui->button_disconnect->setGeometry(x, c->y(), width , c->height());
    ui->button_disconnect->setVisible(true);
}

void PrimaryWindow::closeConnection() {
    ui->label_connection->setText("No connection  ");
    ui->label_connection->setStyleSheet("QLabel { color : orange; }");
    ui->button_disconnect->setVisible(false);
}

void PrimaryWindow::closeConnectionDialog() {
    if (connectionDialog != 0) {
        connectionDialog->accept();
    }
}

void PrimaryWindow::closeEditAttrDialog() {
    if (editAttrDialog != 0) {
        editAttrDialog->accept();
    }
}

void PrimaryWindow::setDialogConnectionError(QString errorText) {
    connectionDialog->setConnectionError(errorText);
}

void PrimaryWindow::quit() {
    closeConnectionDialog();
    qApp->quit();
}

int PrimaryWindow::getMetaIdForRow(int row) {
    return shownAttrIds.at(row);
}

int PrimaryWindow::getShownAttrOwnerId() {
    return shownAttrOwnerId;
}

QString PrimaryWindow::getShownAttrOwnerName() {
    return shownAttrOwnerName;
}

/* slots */

void PrimaryWindow::searchButtonClicked() {
    QString userQuery = ui->lineEdit_query->text();
    QRegularExpression empty("^\\s*$");
    if (empty.match(userQuery).hasMatch()) {
        return;
    }
    controller->searchButtonClicked(userQuery);
}

void PrimaryWindow::disconnectButtonPressed() {
    controller->disconnectButtonPressed();
}

void PrimaryWindow::attributeSearchUpdated(QString text) {
    controller->attributeSearchUpdated(text);
}

void PrimaryWindow::connectionDialogConnectPressed() {
    QString sqlDriver = connectionDialog->getSqlDriver();
    QString host = connectionDialog->getHost();
    QString dbName = connectionDialog->getDbName();
    QString username = connectionDialog->getUsername();
    QString password = connectionDialog->getPassword();
    controller->cdConnectButtonPressed(sqlDriver, host, dbName, username, password);
}

void PrimaryWindow::connectionDialogExitPressed() {
    controller->cdExitButtonPressed();
}

void PrimaryWindow::editAttrDialogAcceptPressed() {
    QString name = editAttrDialog->getName();
    QString value = editAttrDialog->getValue();
    QString units = editAttrDialog->getUnits();
    int metaId = editAttrDialog->getMetaId();
    controller->eadAcceptButtonPressed(metaId, name, value, units);
}

void PrimaryWindow::editAttrDialogCancelPressed() {
    controller->eadCancelButtonPressed();
}

void PrimaryWindow::firstTimeDialog() {
    askForNewConnection();
}

void PrimaryWindow::foundListDoubleClicked(QModelIndex listItem) {
    QString attrName = listItem.data().toString();
    QString attributeQuery = QString("exists %1").arg(attrName);
    ui->lineEdit_query->setText(ui->lineEdit_query->text() + attributeQuery);
}

void PrimaryWindow::resultTableRowDoubleClicked(QModelIndex tableItem) {
    int row = tableItem.row();
    int column = 0;
    QString fileName = ui->table_results->item(row, column)->text();
    int fileId = shownDataIds.at(row);
    controller->resultDoubleClicked(fileId, fileName);
}

void PrimaryWindow::editAttributeClicked() {
    editAttribute();
}

void PrimaryWindow::newAttributeClicked() {
    addNewAttribute();
}

void PrimaryWindow::removeAttributeClicked() {
    QList<int> idsToRemove;
    QModelIndexList select = ui->table_attributes->selectionModel()->selectedRows();
    foreach (QModelIndex mi, select) {
        int row = mi.row();
        idsToRemove.append(shownAttrIds.at(row));
    }
    controller->removeAttributes(idsToRemove);
}
