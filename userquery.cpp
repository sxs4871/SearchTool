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
    /* This expression finds first occurence of correctly balanced parentheses */
    QRegularExpression balancedParentheses("\\((?>[^()]|(?R))*\\)");
    QRegularExpressionMatch match = balancedParentheses.match(fullExpr, index);
    if (match.hasMatch()) {
        matchedText = match.capturedTexts().at(0);
    }
    return matchedText;
}

QString UserQuery::transformSimpleExpr(QString expr) {
    QueryFormatException wrongFormatException("Invalid query: " + expr);
    QString nonEscape("(?<!\\\\)");
    /* Finds =,>,<,>=,<=,exists keys that aren't preceeded by '\' symbol */
    QRegularExpression operatorMatch(nonEscape + "=|[><](=)?|exists");
    bool exactlyOneOperator = true;
    int beginning, end;
    QRegularExpressionMatch match = operatorMatch.match(expr);
    if (! match.hasMatch()) {
        exactlyOneOperator = false;
    } else {
        qDebug() << "First match: " << match.captured();
        beginning = match.capturedStart();
        end = match.capturedEnd();
        match = operatorMatch.match(expr, end);
        if (match.hasMatch()) {
            qDebug() << "Second match: " << match.captured();
            exactlyOneOperator = false;
        }
    }
    if (! exactlyOneOperator) {
        throw wrongFormatException;
    }
    QString op = expr.mid(beginning, end - beginning);
    // At this point, we have split expression into right, left and the operator itself
    // Now, need to parse left and right sides
    QStringList parts = expr.split(op);
    qDebug() << parts;
    assert(parts.size() == 2);
    QString leftSide = parts[0];
    QString rightSide = parts[1];
    bool validFormat = false;
    int unitIndex = -1;
    QString attrName, value, units;
    /*  If operators is 'exists', then it should only have
        right side arguments.
        Else, look for [units] at the end of the right side
        and split right side into value and units */
    if (op == "exists") {
        /* This expression represents whitespace or empty string */
        QRegularExpression whitespace("^(\\s+)?$");
        if (whitespace.match(leftSide).hasMatch()) {
            attrName = rightSide;
            validFormat = true;
        }
    } else {
        attrName = leftSide;
        /* This expression finds first occurence of correctly balanced brackets */
        QRegularExpression balancedBrackets("\\[(?>[^\\[\\]]|(?R))*\\]");
        QRegularExpressionMatch unitsMatch = balancedBrackets.match(rightSide);
        if (unitsMatch.hasMatch()) {
            qDebug() << unitsMatch.capturedTexts();
            QString trailingText = rightSide.mid(unitsMatch.capturedEnd());
            /* This expression represents whitespace or empty string */
            QRegularExpression whiteSpace("^(\\s+)?$");
            if (whiteSpace.match(trailingText).hasMatch()) {
                validFormat = true;
                unitIndex = unitsMatch.capturedStart();
                value = rightSide.mid(0, unitIndex);
                QString bracketedUnits = unitsMatch.capturedTexts().at(0);
                units = bracketedUnits.mid(1, bracketedUnits.size() - 2);
            }
        } else {
            value = rightSide;
            validFormat = true;
        }
    }
    // Here, parsing is done, if format is invalid, throw format exceptions
    if (! validFormat) {
        throw wrongFormatException;
    }
    qDebug() << "Attribute name: " << attrName;
    qDebug() << "Value: " << value;
    qDebug() << "Units: " << units;

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

