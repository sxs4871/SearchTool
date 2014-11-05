#ifndef QUERYFORMATEXCEPTION_H
#define QUERYFORMATEXCEPTION_H

#include <exception>
#include <QString>

class QueryFormatException : public std::exception {

public:
    QueryFormatException(QString errMessage);
    ~QueryFormatException() throw();
    QString getErrorMessage();

private:
    QString errorMessage;
};

#endif // QUERYFORMATEXCEPTION_H
