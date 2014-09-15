#ifndef AVU_H
#define AVU_H

#include <QString>

class AVU
{
public:
    AVU(QString _attrName, QString _value = 0, QString _unit = 0);

    QString operator [](unsigned i);

    QString toString();

private:
    QString attrName;
    QString value;
    QString unit;
};

#endif // AVU_H
