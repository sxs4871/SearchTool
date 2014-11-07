#ifndef QUERYFORMATEXCEPTION_H
#define QUERYFORMATEXCEPTION_H

#include <exception>
#include <QString>
#include <QStringList>

class QueryFormatException : public std::exception {

public:
    QueryFormatException();
    QueryFormatException(QString errMessage);
    ~QueryFormatException() throw();
    void addError(QString errorDescription);

    QStringList getErrorMessages();
    void printErrorMessages();

private:
    QStringList queryErrors;
};

#endif // QUERYFORMATEXCEPTION_H
