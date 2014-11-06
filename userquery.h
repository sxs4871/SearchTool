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
    QString toSQL();
    QString transformQuery(QString query);
    QString transformSimpleExpr(QString expr);
    bool isParenthesized(QString);
    QString getInnerExpression(QString parenthesizedExpr);
    QString formatWhitespace(QString expr);
    QString generateSQLCondition(QString attribute, QString value, QString units);
    QString getParenthesizedExpr(QString fullExpr, int index);

    QString transformUnparenthesizedExpr(QString expr);
private:
    // fields
    QString queryString;

    // methods


};

#endif // USERQUERY_H
