#ifndef USERQUERY_H
#define USERQUERY_H

#include <QString>
#include <QHash>
#include <queryformatexception.h>

class UserQuery
{

public:
    // methods
    UserQuery(QString _userQuery);
    QString toSQL() throw(QueryFormatException);
    QString transformQuery(QString query);
    QString transformGroup(QString group) throw(QueryFormatException);
    bool isParenthesizedGroup(QString group);
    QString getInnerExpression(QString parenthesizedGroup);
    QString formatWhitespace(QString expr);
    QString removePaddingWhitespace(QString expr);
    QString generateSQLCondition(QString attribute, QString value, QString units);

private:
    // fields
    QString queryString;
    QueryFormatException formatException;
    // methods


};

#endif // USERQUERY_H
