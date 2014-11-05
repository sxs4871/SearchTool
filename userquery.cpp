#include "userquery.h"
#include <QStringList>
#include <QStringBuilder>
#include <QDebug>
#include <QRegularExpression>
#include "assert.h"



UserQuery::UserQuery(QString _queryString) {
    queryString = _queryString;
    operators = new QStringList();
    connectors = new QStringList();
    (*operators) << "=" << ">" << "<" << ">=" << "<=";
    (*connectors) << "AND" << "OR" << "||" << "&&";
}

/**
 * Step 1: Recursively evaluate all parenthesized expressions
 * Step 2: For unparenthesized expressions, split it by AND/OR connectors
 * Step 3: In Simple split expressions find single operator (=,<,>,<=,>=)
 * Step 4: Find metadata pieces around operator
 * Step 5: Rewrite these simple expressions as SQL conditions
 * @brief UserQuery::toSQL
 * @return
 */
QString UserQuery::toSQL() {
    QString condition = queryString;
    QString sqlBase = "SELECT DISTINCT d.data_name, d.resc_group_name, d.modify_ts FROM r_data_main d, r_meta_main m, r_objt_metamap om WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and (";
    sqlBase = sqlBase + condition + ")";

    QRegularExpression balancedParentheses("\\((?>[^()]|(?R))*\\)");
    bool hasParentheses = true;
    int index = 0;
    while (hasParentheses) {
        QRegularExpressionMatch match = balancedParentheses.match(condition, index);
        if ( match.hasMatch()) {
            int firstParenthesisIndex = condition.indexOf("(", index);
            if (match.capturedStart() != firstParenthesisIndex) {
                throw (QueryFormatException("Unclosed parenthesis at index " + QString::number(firstParenthesisIndex)));
            } else {
                QString parenthesizedExpr = match.captured();
                qDebug() << "Found parenthesized expr: " + parenthesizedExpr;
                index = match.capturedEnd();
            }
        } else {
            hasParentheses = false;
        }
    }
    int closingIndex = condition.indexOf(")", index);
    if (closingIndex != -1) {
        throw (QueryFormatException("Unmatched closing parenthesis at index: " + QString(closingIndex)));
    }


    return sqlBase;
}

QString UserQuery::getParenthesizedExpr(QString fullExpr, int index) {
    QString matchedText;
    QRegularExpression balancedParentheses("\\((?>[^()]|(?R))*\\)");
    QRegularExpressionMatch match = balancedParentheses.match(fullExpr, index);
    if (match.hasMatch()) {
        matchedText = match.capturedTexts().at(0);
    }
    return matchedText;
}

QString UserQuery::transformSingleExpr(QString expr) {
    return expr;
}

QStringList UserQuery::getExprParts(QString expr) {
    // [0 = operator, 1 = name, 2 = value, 3 = unit]
    QStringList parts;

    QRegularExpression operators(" !/= | !/> | !/< | !/>= | !/<= ");
    qDebug() << operators.match(expr).capturedTexts();

    return parts;
}

























//QString UserQuery::toSQL() {
//    QString condition = queryString;
//    QString sqlBase = "SELECT DISTINCT d.data_name, d.resc_group_name, d.modify_ts FROM r_data_main d, r_meta_main m, r_objt_metamap om WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and (";

//    QRegularExpression a("(?<!\\w)[aA](?!\\w)");
//    QRegularExpression v("(?<![a-zA-Z])[vV](?![a-zA-Z])");
//    QRegularExpression u("(?<![a-zA-Z])[uU](?![a-zA-Z])");
//    condition.replace(a, "m.meta_attr_name");
//    condition.replace(v, "m.meta_attr_value");
//    condition.replace(u, "m.meta_attr_unit");
//    sqlBase = sqlBase + condition + ")";
//    return sqlBase;
//}

