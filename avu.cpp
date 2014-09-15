#include "avu.h"
#include "stdexcept"

AVU::AVU(QString _attrName, QString _value, QString _unit) :
    attrName(_attrName), value(_value), unit(_unit) {
}

QString AVU::operator [](unsigned i) {
    switch (i) {
        case (0) :
            return attrName;
            break;
        case (1):
            return value;
            break;
        case (2):
            return unit;
            break;
    }
    throw std::out_of_range ("AVUs have range 0-2.");
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
