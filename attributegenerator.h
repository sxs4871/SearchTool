#ifndef ATTRIBUTEGENERATOR_H
#define ATTRIBUTEGENERATOR_H

#include "irodsquerymodel.h"
#include "QMultiHash"

class AttributeGenerator
{
public:
    AttributeGenerator(iRODSQueryModel* _model);
    QList<AVU*> generateAttributes(int amount);

private:
    QHash<QString, QList<QString>*>* getKnownUnits();
    iRODSQueryModel* model;
    QHash<QString, QList<QString>*>* units;
};

#endif // ATTRIBUTEGENERATOR_H
