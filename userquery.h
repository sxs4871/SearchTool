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
    QString transformUnparenthesizedExpr(QString expr);
    QString transformSimpleExpr(QString expr);
    QStringList getExprParts(QString expr);
    QStringList splitExpression(QString fullExpr);
    QString getParenthesizedExpr(QString fullExpr, int index);

private:
    // fields
    QString queryString;

    // methods


};

#endif // USERQUERY_H
