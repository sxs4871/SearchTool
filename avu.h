#ifndef AVU_H
#define AVU_H

#include <QString>

class AVU
{
public:
    /* Methods */
    AVU(QString _attrName, QString _value = 0, QString _unit = 0);
    QString toString();
    QString getAttrName();
    QString getValue();
    QString getUnit();

private:
    /* Fields */
    QString attrName;
    QString value;
    QString unit;

};

#endif // AVU_H
