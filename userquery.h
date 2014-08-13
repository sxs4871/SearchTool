#ifndef USERQUERY_H
#define USERQUERY_H

#include <QString>
#include <QHash>

class UserQuery
{
public:
    UserQuery(QString _userQuery);
    QString toSQL();
private:
    QString queryString;
};

#endif // USERQUERY_H
