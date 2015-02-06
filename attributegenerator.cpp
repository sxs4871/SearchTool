#include "attributegenerator.h"

AttributeGenerator::AttributeGenerator(iRODSQueryModel* _model) {
    model = _model;
    units = getKnownUnits();
}

QList<AVU*> AttributeGenerator::generateAttributes(int amount) {
    QList<AVU*> avus;
    QList<QString>  keys = units->keys();
    for (int i = 0; i < amount; i++) {
        int index = qrand() % keys.length();
        QString category = keys.at(index);
        QList<QString>* unitOptions = units->value(keys.at(index));
        QString unit = unitOptions->at(qrand() % unitOptions->length());
        int valueBoundary = qrand() % 1000;
        AVU* newAVU = new AVU(category, QString::number(qrand() % valueBoundary), unit);
        avus.append(newAVU);
    }
    return avus;
}

QHash<QString, QList<QString>*>* AttributeGenerator::getKnownUnits() {
    QString path_src = QString(PROJECT_PATH) + "/units.txt";
    QFile source(path_src);
    QHash<QString, QList<QString>*>* units = new QHash<QString, QList<QString>*>();
    if (source.open(QIODevice::ReadOnly)) {
        QTextStream in(&source);
        while ( !in.atEnd() ) {
            QString unit = in.readLine();
            QString category = in.readLine();
            if (! units->contains(category)) {
                QList<QString>* list = new QList<QString>();
                units->insert(category, list);
            }
            units->value(category)->append(unit);
        }
    }
    return units;
}
