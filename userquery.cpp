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
    QString sqlCondition(condition);
    QString sqlQuery = QString("SELECT DISTINCT d.data_name, d.resc_group_name, d.modify_ts \
            FROM r_data_main d, r_meta_main m, r_objt_metamap om \
            WHERE m.meta_id=om.meta_id and d.data_id=om.object_id and (%1)").arg(sqlCondition);
    return sqlQuery;
}

QString UserQuery::transformQuery(QString query) {
    QString sqlCondition;
    QStringList connectors, parts;
    QRegularExpression balancedParentheses("\\((?>[^()]|(?R))*\\)");
    QRegularExpression findConnectors("(^|(?<=[\\s)]))(AND|OR)($|(?=[\\s(]))");

    int prevGroupEnd = 0, nextGroupStart = query.size(), lastConnectorEnd = 0;
    QRegularExpressionMatch groupMatch = balancedParentheses.match(query);
    if (groupMatch.hasMatch()) {
        nextGroupStart = groupMatch.capturedStart();
    }
    QRegularExpressionMatch connectorMatch = findConnectors.match(query);
    while (connectorMatch.hasMatch()) {
        qDebug() << "ConnectorMatch: " << connectorMatch.captured();
        qDebug() << "GroupMatch: " << groupMatch.captured();
        if ( (connectorMatch.capturedEnd() >= prevGroupEnd)
             && (connectorMatch.capturedEnd() <= nextGroupStart) ) {
            QString connector(connectorMatch.captured());
            QString leftPart = query.mid(lastConnectorEnd, connectorMatch.capturedStart() - lastConnectorEnd);
            connectors << connector;
            parts << leftPart;
            lastConnectorEnd = connectorMatch.capturedEnd();
        } else {
            while (! (nextGroupStart >= connectorMatch.capturedEnd())) {
                prevGroupEnd = groupMatch.capturedEnd();
                groupMatch = balancedParentheses.match(query, prevGroupEnd);
                if (groupMatch.hasMatch()) {
                    nextGroupStart = groupMatch.capturedStart();
                } else {
                    nextGroupStart = query.size();
                }
            }
        }
        connectorMatch = findConnectors.match(query, connectorMatch.capturedEnd());
    }
    QString remainder = query.mid(lastConnectorEnd);
    parts << remainder;
    qDebug() << "Parts: " << parts;
    qDebug() << "Connectors: " << connectors;

    return sqlCondition;

//    bool hasParentheses = true;
//    int index = 0;
//    while (hasParentheses) {
//        QRegularExpressionMatch match = balancedParentheses.match(query, index);
//        if ( match.hasMatch()) {
//            int firstParenthesisIndex = query.indexOf("(", index);
//            if (match.capturedStart() != firstParenthesisIndex) {
//                throw (QueryFormatException("Unclosed parenthesis at index " + QString::number(firstParenthesisIndex)));
//            } else {
//                QString parenthesizedExpr = match.captured();
//                qDebug() << "Found parenthesized expr: " + parenthesizedExpr;
//                index = match.capturedEnd();
//            }
//        } else {
//            hasParentheses = false;
//        }
//    }
//    int closingIndex = query.indexOf(")", index);
//    if (closingIndex != -1) {
//        throw (QueryFormatException("Unmatched closing parenthesis at index: " + QString(closingIndex)));
//    }
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
