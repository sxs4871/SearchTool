#include "userquery.h"
#include <QStringList>
#include <QStringBuilder>
#include <QDebug>
#include <QRegularExpression>

UserQuery::UserQuery(QString _queryString) {
    queryString = _queryString;
}

QString UserQuery::toSQL() {
    QString condition = queryString;
    QString sqlBase = "SELECT DISTINCT d.data_name, d.resc_group_name, d.modify_ts FROM r_data_main d, r_meta_main m, r_objt_metamap om WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and (";

    QRegularExpression a("(?<!\\w)[aA](?!\\w)");
    QRegularExpression v("(?<![a-zA-Z])[vV](?![a-zA-Z])");
    QRegularExpression u("(?<![a-zA-Z])[uU](?![a-zA-Z])");
    condition.replace(a, "m.meta_attr_name");
    condition.replace(v, "m.meta_attr_value");
    condition.replace(u, "m.meta_attr_unit");
    sqlBase = sqlBase + condition + ")";
    return sqlBase;
}

