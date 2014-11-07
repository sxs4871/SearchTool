#include "queryformatexception.h"
#include <QDebug>

QueryFormatException::QueryFormatException() {
    // no errors logged
}

QueryFormatException::QueryFormatException(QString errMessage) {
    queryErrors << errMessage;
}

QueryFormatException::~QueryFormatException() throw() {
}

void QueryFormatException::addError(QString errorDescription) {
    queryErrors << errorDescription;
}

QStringList QueryFormatException::getErrorMessages() {
    return queryErrors;
}

void QueryFormatException::printErrorMessages() {
    foreach(QString error, queryErrors) {
        qDebug() << error;
    }
}
