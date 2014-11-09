#include "avu.h"
#include "stdexcept"

AVU::AVU(QString _attrName, QString _value, QString _unit) :
    attrName(_attrName), value(_value), unit(_unit) {
}

QString AVU::toString() {
    QString str = "attribute: \'" + attrName + "\'";
    if (value != 0) {
        str = str + ", value: \'" + value + "\'";
    }
    if (unit != 0) {
        str = str + ", unit: \'" + unit + "\'";
    }
    return str;
}

QString AVU::getAttrName() {
    return attrName;
}

QString AVU::getValue() {
    return value;
}

QString AVU::getUnit() {
    return unit;
}
