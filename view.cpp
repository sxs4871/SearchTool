#include "view.h"
#include <connectiondialog.h>

View::View(Ui::MainWindow* _ui, Controller* _c) {
    if (_ui == 0) {
        new Ui::MainWindow();
    } else {
        ui = _ui;
    }
    controller = _c;
    initView();
}

View::~View() {}

void View::initView() {
    model = Model::getInstance();

    ui->frame_setValue->setStyleSheet("QFrame {border : 0px solid black; }");

    /* GroupBox setup */
    QString groupBoxStyle("QGroupBox {font-size: 14px; font-weight: bold;border: 1px solid gray;border-radius: 5px;margin-top: 0.5em;}\
            QGroupBox::title {subcontrol-origin: margin;left: 10px;padding: 0 3px 0 3px;}");
    ui->group_findAttr->setStyleSheet(groupBoxStyle);
    ui->group_selectedAttr->setStyleSheet(groupBoxStyle);
    ui->group_setVal->setStyleSheet(groupBoxStyle);

    /* LineEdit setup */
    ui->lineEdit_findAttr->setStyleSheet("QLineEdit { border: 1px solid gray; border-radius: 10px; }");
    ui->lineEdit_query->setStyleSheet("QLineEdit { border: 1px solid gray; border-radius: 13px; }");
    ui->lineEdit_findAttr->setPlaceholderText("  Find attribute...");
    ui->lineEdit_query->setPlaceholderText("   Enter your query...");
    ui->lineEdit_enterUnit->setPlaceholderText("Enter units...");
    ui->lineEdit_enterVal->setPlaceholderText("Enter value...");

    /* Table of results setup */
    ui->table_results->setColumnCount(3);
    ui->table_results->setHorizontalHeaderLabels(QStringList() << "Size" << "Collection" << "Date modified");
    ui->table_results->horizontalHeader()->setVisible(true);
    int tableWidth = ui->table_results->width();
    int colCount = ui->table_results->columnCount();
    for (int i = 0; i < colCount; i++) {
        ui->table_results->setColumnWidth(i, tableWidth / 3);
    }
    ui->table_results->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->label_queryMessage->setHidden(true);
    ui->label_queryMessage->setFixedWidth(ui->table_results->width() - 20);
    ui->label_queryMessage->setWordWrap(true);

    /* Lists setup */
    ui->list_selectedAttr->setStyleSheet("background-color: transparent;");
    ui->list_foundAttr->setStyleSheet("background-color: transparent;");


    /* connecting signals and slots */
    QObject::connect(ui->lineEdit_query, SIGNAL(returnPressed()), ui->button_search, SIGNAL(clicked()));
    QObject::connect(ui->button_search, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
    QObject::connect(ui->lineEdit_findAttr, SIGNAL(textChanged(QString)), this, SLOT(attributeSearchUpdated(QString)));
    QObject::connect(ui->button_disconnect, SIGNAL(clicked()), this, SLOT(disconnectButtonPressed()));
    QObject::connect(model, SIGNAL(searchCompleted(QSqlQuery)), this, SLOT(modelFinishedSearch(QSqlQuery)));
    QObject::connect(model, SIGNAL(foundAttributes(QStringList)), this, SLOT(modelFoundAttributes(QStringList)));

    askForNewConnection();
}

Ui::MainWindow* View::getUI() {
    return ui;
}

void View::askForNewConnection() {
    ConnectionDialog cd;
    cd.setModal(true);
    cd.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    cd.setDefaults();
    if (cd.exec() == QDialog::Accepted) {
        model = Model::getInstance();
        model->readAllAttributes();
        this->attributeSearchUpdated("");
        QLabel* c = ui->label_connection;
        c->setText("Connected to:  " + model->getDbName());
        c->setStyleSheet("QLabel { color : green; }");
        c->adjustSize();
        c->setFixedHeight(27);
        int x = c->x() + c->width() + 20;
        int width = ui->group_setVal->x() + ui->group_setVal->width() - x;
        ui->button_disconnect->setGeometry(x, c->y(), width , c->height());
        ui->button_disconnect->setVisible(true);
    }
}

/* slots */

void View::searchButtonClicked() {
    QString userQuery = ui->lineEdit_query->text();
    if (userQuery == "") {
        return;
    }
    controller->searchButtonClicked(userQuery);
}

void View::disconnectButtonPressed() {
    model->disconnect();
    ui->label_connection->setText("No connection  ");
    ui->label_connection->setStyleSheet("QLabel { color : orange; }");
    ui->button_disconnect->setVisible(false);
    askForNewConnection();
}

void View::attributeSearchUpdated(QString text) {
    controller->attributeSearchUpdated(text);
}

void View::modelFinishedSearch(QSqlQuery result) {
    qDebug() << "Signal received";

    ui->table_results->setRowCount(0);
    bool ok = result.isActive();
    if (!ok) {
        ui->label_queryMessage->setText("<font color=\"red\">Query error:</font><p></p>" + result.lastError().databaseText());
        ui->label_queryMessage->adjustSize();
        ui->label_queryMessage->setHidden(false);
        return;
    }

    int resultCount = result.size();
    if (resultCount == 0) {
        ui->label_queryMessage->setText("<font color=\"orange\">No results found</font><p></p>");
        ui->label_queryMessage->adjustSize();
        ui->label_queryMessage->setHidden(false);
        return;
    }
    ui->label_queryMessage->setHidden(true);
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
        lastRow++;
    }
}

void View::modelFoundAttributes(QStringList attributes) {
    attributes.sort();
    ui->list_foundAttr->clear();
    ui->list_foundAttr->addItems(attributes);
}
