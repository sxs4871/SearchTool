#ifndef USERQUERY_H
#define USERQUERY_H

#include <QString>
#include <QHash>
#include <queryformatexception.h>

/**
 * @brief The UserQuery class
 * This class is created for the purpose of storing user's query
 * in custom format and translating this query into an SQL statement
 */
class UserQuery
{

public:
    /**
     * @brief UserQuery
     * Create a query with a query string in user's format
     * @param _userQuery - text of the query
     */
    UserQuery(QString _userQuery);

    /**
     * @brief toSQL - translate query from local format to SQL
     * Any errors encountered during the translation process are accumulated in the
     * exception. At the end of translation, if exception object holds errors,
     * QueryFormat exception is thrown. Otherwise, translated statement is returned.
     * @return - SQL statement
     * @throw - QueryFormatException, with all encountered format errors
     */
    QString toSQL() throw(QueryFormatException);

private:
    /* Container for local format query */
    QString queryString;
    /* Exception object used for storing errors in the format
     * that were found throughout the process of translating query
     * into SQL. */
    QueryFormatException formatException;

    /**
     * @brief transformQuery
     * Translates query in local format to an SQL statement.
     * Algorithm high-level description: query in local format consists
     * of multiple statements connected by logical operators (AND, OR, etc.)
     * Steps: 1. Split the whole query into individual statements
     *           that are connected by logical operations.
     *        2. Each of those statements can either be a single statement (name=value [unit]),
     *           or a whole other subquery, in which case it will be wrapped in parentheses.
     *        3(a). If such statement, is a single statement, then proceed to checking
     *              its format and extracting 'attribute name', 'value' and 'unit' from it.
     *        3(b). If statement is a parenthesized expression (subquery), make a recursive call
     *              with this same method to translate it (treat it a whole different new query)
     *        4. Stitch translated individual results back together using those logical operators
     *           that were found between them.
     * @param query - query in the local format
     * @return - translated statement
     */
    QString transformQuery(QString query);

    /**
     * @brief transformGroup - 'Group' here refers to an single statement (name = value [unit])
     * This means that there are no parentheses in this expession. This method checks format
     * according to the local format of such individual format and, if format is good, exctracts
     * name, value and unit values from this expression.
     * @param group - single statement in local format
     * @return - SQL condition for this statement (group)
     * @throw - QueryFormatException, if format isn't right
     */
    QString transformGroup(QString group) throw(QueryFormatException);

    /**
     * @brief isParenthesizedGroup
     * Checks if given expression is fully contained inside parentheses
     * @param group - expression to check
     * @return true, if the whole given expression is inside parentheses
     *         false, otherwise
     */
    bool isParenthesizedGroup(QString group);

    /**
     * @brief getInnerExpression
     * Assumes that given string is an expression, fully inside parentheses, and
     * extracts expression that is inside those parentheses. However, if contract is violated,
     * only the expression from within first matched parentheses will be returned.
     * @param parenthesizedGroup - expression inside parentheses.
     * @return - text inside outer parentheses
     */
    QString getInnerExpression(QString parenthesizedGroup);

    /**
     * @brief formatWhitespace
     * This method cleans up the query by removing leading spaces, trailing spaces
     * and substituting multiple spaces with just one.
     * @param expr - expression to clean up
     * @return - cleaned expression
     */
    QString formatWhitespace(QString expr);

    /**
     * @brief removePaddingWhitespace - removes leading and trailing whitespace
     * @param expr - expression to clean up
     * @return - cleaned expression
     */
    QString removePaddingWhitespace(QString expr);

    /**
     * @brief generateSQLCondition - creates part of the sql query, which
     * will request objects that match given name, value and units. This SQL
     * condition is later inserted in the complete SQL statement. Empty values
     * are ignored and aren't included in the condition.
     * @param attribute - name of the attribute
     * @param value - value of the attribute
     * @param units - attribute units
     * @return - SQL condition to match given name, value and units.
     */
    QString generateSQLCondition(QString attribute, QString value, QString units);

};

#endif // USERQUERY_H
