#include "queryformatexception.h"

QueryFormatException::QueryFormatException(QString errMessage) {
    errorMessage = errMessage;
}

QueryFormatException::~QueryFormatException() throw() {
}

QString QueryFormatException::getErrorMessage() {
    return errorMessage;
}
