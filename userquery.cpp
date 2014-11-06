#include "userquery.h"
#include <QStringList>
#include <QStringBuilder>
#include <QDebug>
#include <QRegularExpression>
#include "assert.h"



UserQuery::UserQuery(QString _queryString) {
    queryString = _queryString;
}

/**
 * Step 1: Recursively evaluate all parenthesized expressions
 * Step 2: For unparenthesized expressions, split it by AND/OR connectors
 * Step 3: In Simple split expressions find sing;le operator (=,<,>,<=,>=)
 * Step 4: Find metadata pieces around operator
 * Step 5: Rewrite these simple expressions as SQL conditions
 * @brief UserQuery::toSQL
 * @return
 */
QString UserQuery::toSQL() {
    QString condition = queryString;
    condition = formatWhitespace(condition);
    QString sqlCondition(condition);
    QString sqlQuery = QString("SELECT DISTINCT d.data_name, d.resc_group_name, d.modify_ts \
            FROM r_data_main d, r_meta_main m, r_objt_metamap om \
            WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and (%1)").arg(sqlCondition);
    return sqlQuery;
}

QString UserQuery::transformQuery(QString query) {
    qDebug() << "______________________________";
    qDebug() << "Evaluating '" + query + "'";
    QStringList connectors, parts;
    /* Corresponds to a properly balanced parenthesized text */
    QRegularExpression balancedParentheses("\\((?>[^()]|(?R))*\\)");
    /* Corresponds to 'AND' or 'OR' that is preceeded by space, closing parentheses or start of line,
     * and followed by another space, opening parentheses or end of line */
    QRegularExpression findConnectors("(^|(?<=[\\s)]))(AND|OR)($|(?=[\\s(]))");

    int prevGroupEnd = 0, nextGroupStart = query.size(), lastConnectorEnd = 0;
    QRegularExpressionMatch groupMatch = balancedParentheses.match(query);
    if (groupMatch.hasMatch()) {
        nextGroupStart = groupMatch.capturedStart();
    }
    QRegularExpressionMatch connectorMatch = findConnectors.match(query);
    while (connectorMatch.hasMatch()) {
        while (! (nextGroupStart >= connectorMatch.capturedEnd())) {
            prevGroupEnd = groupMatch.capturedEnd();
            groupMatch = balancedParentheses.match(query, prevGroupEnd);
            if (groupMatch.hasMatch()) {
                nextGroupStart = groupMatch.capturedStart();
            } else {
                nextGroupStart = query.size();
            }
        }
        if ( (connectorMatch.capturedEnd() >= prevGroupEnd)
             && (connectorMatch.capturedEnd() <= nextGroupStart) ) {
            QString connector(connectorMatch.captured());
            QString leftPart = query.mid(lastConnectorEnd, connectorMatch.capturedStart() - lastConnectorEnd);
            connectors << connector;
            parts << leftPart;
            lastConnectorEnd = connectorMatch.capturedEnd(); 
        }
        connectorMatch = findConnectors.match(query, connectorMatch.capturedEnd());
    }
    QString remainder = query.mid(lastConnectorEnd);
    parts << remainder;

    qDebug() << "Parts: " << parts;
    qDebug() << "Connectors: " << connectors;
    assert (parts.size() != 0);
    assert (parts.size() == connectors.size() + 1);

    QString sqlCondition;
    if (connectors.size() == 0) {
        try {
            sqlCondition += transformSimpleExpr(parts[0]);
        } catch (QueryFormatException e) {
            qDebug() << e.getErrorMessage();
        }
    } else {
        try {
            for (int i = 0; i < connectors.size(); i++) {
                QString nextPartSQL = transformSimpleExpr(parts[i]);
                QString nextConnector = connectors[i];
                sqlCondition += QString("%1 %2 ").arg(nextPartSQL, nextConnector);
            }
            sqlCondition += transformSimpleExpr(parts.last());
        } catch (QueryFormatException e) {
            qDebug() << e.getErrorMessage();
        }
        sqlCondition = QString("(%1)").arg(sqlCondition);
    }


    qDebug() << "Full SQL Condition: " + sqlCondition;
    return sqlCondition;

}

/**
 * @brief UserQuery::transformSimpleExpr
 * Accepts a simple expression (not connected by logical connectors),
 * and returns an SQL condition for it
 * @param expr
 * @return
 */
QString UserQuery::transformSimpleExpr(QString expr) {
    qDebug() << "______________________________";
    qDebug() << "Evaluating '" + expr + "'";
    if (isParenthesized(expr)) {
        QString innerExpr = getInnerExpression(expr);
        qDebug() << "Parenthesized expression detected: " << expr;
        qDebug() << "Inner expression is: " << innerExpr;
        return transformQuery(innerExpr);
    }
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
        beginning = match.capturedStart();
        end = match.capturedEnd();
        match = operatorMatch.match(expr, end);
        if (match.hasMatch()) {
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

    QString sqlCondition = generateSQLCondition(attrName, value, units);
    qDebug() << "SQL Format: " +sqlCondition;

    return sqlCondition;
}

bool UserQuery::isParenthesized(QString expr) {
    QRegularExpression parenthesizedExpr("^\\s+\\((?>[^()]|(?R))*\\)\\s+$");
    return (parenthesizedExpr.match(expr).hasMatch());
}

QString UserQuery::getInnerExpression(QString parenthesizedExpr) {
    QRegularExpression matchParenthesized("\\((?>[^()]|(?R))*\\)");
    QRegularExpressionMatch match = matchParenthesized.match(parenthesizedExpr);
    QString innerExpr;
    if (match.hasMatch()) {
        innerExpr = parenthesizedExpr.mid(match.capturedStart() + 1, match.capturedLength() - 2);
    }
    return innerExpr;
}

QString UserQuery::formatWhitespace(QString expr) {
    QRegularExpression leadingWhiteSpace("^\\s+");
    QRegularExpression trailingWHiteSpace("\\s+$");
    QRegularExpression multipleWhiteSpace("\\s{2,}");
    expr = expr.replace(leadingWhiteSpace, "");
    expr = expr.replace(trailingWHiteSpace, "");
    expr = expr.replace(multipleWhiteSpace, "  ");
    return expr;
}

QString UserQuery::generateSQLCondition(QString name, QString value, QString units) {
    QString fullCondition;
    QRegularExpression fullWhitespace("^\\s*$");
    if (! fullWhitespace.match(name).hasMatch()) {
        if (fullCondition.size() != 0) {
            fullCondition += " AND ";
        }
        QString nameCondition = QString("m.meta_attr_name='%1'").arg(name);
        fullCondition += nameCondition;
    }
    if (! fullWhitespace.match(value).hasMatch()) {
        if (fullCondition.size() != 0) {
            fullCondition += " AND ";
        }
        QString valueCondition = QString("m.meta_attr_value='%1'").arg(value);
        fullCondition += valueCondition;
    }
    if (! fullWhitespace.match(units).hasMatch()) {
        if (fullCondition.size() != 0) {
            fullCondition += " AND ";
        }
        QString unitCondition = QString("m.meta_attr_unit='%1'").arg(units);
        fullCondition += unitCondition;
    }
    fullCondition = QString("(%1)").arg(fullCondition);
    return fullCondition;
}

QString UserQuery::transformUnparenthesizedExpr(QString expr) {
    /* since expr can't have parentheses by contract, we don't have to worry
     * about parentheses before or after logical connector, therefore connectors
     * are going to be surrounded by whitespace to stand out */
    QRegularExpression findConnectors("\\s+(AND|OR)\\s+");
    QStringList connectors, parts;
    QRegularExpressionMatch match = findConnectors.match(expr);
    int prevEnd = 0;
    while (match.hasMatch()) {
        QString connector(match.captured());
        QString leftPart = expr.mid(prevEnd, match.capturedStart() - prevEnd);
        connectors << connector;
        parts << leftPart;
        prevEnd = match.capturedEnd();
        match = findConnectors.match(expr, prevEnd);
    }
    QString remainder = expr.mid(prevEnd);
    parts << remainder;
    qDebug() << "Parts: " << parts;
    qDebug() << "Connectors: " << connectors;
    return expr;
}
