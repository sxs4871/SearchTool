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
 * @brief UserQuery::toSQL
 * @return
 */
QString UserQuery::toSQL() throw(QueryFormatException) {
    formatException = QueryFormatException();
    QString sqlCondition = transformQuery(queryString);
    QString sqlQuery = QString("SELECT DISTINCT d.data_name, d.resc_group_name, d.modify_ts "
                               "FROM r_data_main d, r_meta_main m, r_objt_metamap om "
                               "WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and (%1)").arg(sqlCondition);
    if (formatException.getErrorMessages().size() == 0) {
        return sqlQuery;
    } else {
        formatException.printErrorMessages();
        throw formatException;
    }
}

/**
 * @brief UserQuery::transformQuery
 * Step 1: Split query into groups (parts connected by logical operators)
 * Step 2: Transform each group individually
 *  - If group is a parenthesized expression, evaluate it in a recursive call (Step 1),
 *      else, step 3
 * Step 3: Extract attribute name, value and units from group
 * Step 4: Transform group into SQL condition
 * @param query
 * @return
 */
QString UserQuery::transformQuery(QString query) {
    qDebug() << "______________________________";
    qDebug() << "Evaluating query: " + query;
    QStringList connectors, parts;
    /* Corresponds to a properly balanced parenthesized text */
    QRegularExpression balancedParentheses("\\((?>[^()]|(?R))*\\)");
    /* Corresponds to 'AND' or 'OR' that is preceeded by space, closing parentheses or start of line,
     * and followed by another space, opening parentheses or end of line */
    QRegularExpression findConnectors("(^|(?<=[\\s)]))(AND|OR|&&|\\|\\|)($|(?=[\\s(]))");
    findConnectors.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

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
            sqlCondition += transformGroup(parts[0]);
        } catch (QueryFormatException e) {

        }
    } else {
        for (int i = 0; i < connectors.size(); i++) {
            try {
                QString nextPartSQL = transformGroup(parts[i]);
                QString nextConnector = connectors[i];
                sqlCondition += QString("(%1) %2 ").arg(nextPartSQL, nextConnector);
            } catch (QueryFormatException e) {}
        }
        try {
            sqlCondition += QString("(%1)").arg(transformGroup(parts.last()));
        } catch (QueryFormatException e) {}
    }

    qDebug() << "\n\n";
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
QString UserQuery::transformGroup(QString group) throw(QueryFormatException) {
    qDebug() << "\n______________________________";
    qDebug() << "Evaluating group: " + group;
    group = formatWhitespace(group);
    if (isParenthesizedGroup(group)) {
        QString innerExpr = getInnerExpression(group);
        qDebug() << "Parenthesized expression detected: " << group;
        qDebug() << "Inner expression is: " << innerExpr;
        return transformQuery(innerExpr);
    }
    QString invalidGroupMessage = QString("Invalid group '%1'").arg(group);
    QString nonEscape("(?<!\\\\)");
    /* Finds =,>,<,>=,<=,exists keys that aren't preceeded by '\' symbol */
    QRegularExpression operatorMatch(nonEscape + "=|[><](=)?|exists");
    operatorMatch.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    bool exactlyOneOperator = true;
    int beginning, end;
    QRegularExpressionMatch match = operatorMatch.match(group);
    if (! match.hasMatch()) {
        exactlyOneOperator = false;
    } else {
        beginning = match.capturedStart();
        end = match.capturedEnd();
        match = operatorMatch.match(group, end);
        if (match.hasMatch()) {
            exactlyOneOperator = false;
        }
    }
    if (! exactlyOneOperator) {
        formatException.addError(invalidGroupMessage);
        throw QueryFormatException();
    }
    QString op = group.mid(beginning, end - beginning);
    // At this point, we have split expression into right, left and the operator itself
    // Now, need to parse left and right sides
    QStringList parts = group.split(op);
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
        formatException.addError(invalidGroupMessage);
        throw QueryFormatException();
    }

    QString sqlCondition = generateSQLCondition(attrName, value, units);
    qDebug() << "SQL Condition: " + sqlCondition;
    qDebug() << "______________________";

    return sqlCondition;
}

bool UserQuery::isParenthesizedGroup(QString group) {
    QRegularExpression parenthesizedExpr("^\\s+\\((?>[^()]|(?R))*\\)\\s+$");
    return (parenthesizedExpr.match(group).hasMatch());
}

QString UserQuery::getInnerExpression(QString parenthesizedGroup) {
    QRegularExpression matchParenthesized("\\((?>[^()]|(?R))*\\)");
    QRegularExpressionMatch match = matchParenthesized.match(parenthesizedGroup);
    QString innerExpr;
    if (match.hasMatch()) {
        innerExpr = parenthesizedGroup.mid(match.capturedStart() + 1, match.capturedLength() - 2);
    }
    return innerExpr;
}

QString UserQuery::formatWhitespace(QString expr) {
    QRegularExpression multipleWhiteSpace("\\s{2,}");
    expr = removePaddingWhitespace(expr);
    expr = expr.replace(multipleWhiteSpace, "  ");
    return expr;
}

QString UserQuery::removePaddingWhitespace(QString expr) {
    QRegularExpression leadingWhiteSpace("^\\s+");
    QRegularExpression trailingWHiteSpace("\\s+$");
    expr = expr.replace(leadingWhiteSpace, "");
    expr = expr.replace(trailingWHiteSpace, "");
    return expr;
}

QString UserQuery::generateSQLCondition(QString name, QString value, QString units) {
    name = removePaddingWhitespace(name);
    value = removePaddingWhitespace(value);
    units = removePaddingWhitespace(units);
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
    return fullCondition;
}
