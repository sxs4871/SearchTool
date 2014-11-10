#ifndef QUERYFORMATEXCEPTION_H
#define QUERYFORMATEXCEPTION_H

#include <exception>
#include <QString>
#include <QStringList>

/**
 * @brief The QueryFormatException class
 * Custom exception class used for indicating
 * and tracking errors in the user's query.
 * It is used differently throughout this application than
 * regular exception. Whenever this exception is caught in the
 * query evaluation process, it 'accumulates' error messages to
 * allow continued query evaluation. In the end, if query format
 * was violated, this exception is thrown with a whole list of
 * different errors that were found in the query.
 */
class QueryFormatException : public std::exception {

public:
    /**
     * @brief QueryFormatException
     * Create clean exception object (didn't encounter any errors yet)
     */
    QueryFormatException();

    /**
     * @brief QueryFormatException
     * Creates this exception object for a particular error.
     * @param errMessage - error encountered
     */
    QueryFormatException(QString errMessage);

    /* std::exception inheritance requries subclass destructors to declare
     * thrown exceptions */
    ~QueryFormatException() throw();

    /**
     * @brief addError
     * Adds a new entry to the list of encountered format errors
     * @param errorDescription - what was the error ?
     */
    void addError(QString errorDescription);

    /**
     * @brief getErrorMessages - accessor for list of errors
     * @return - list of all recorded errors
     */
    QStringList getErrorMessages();

    /**
     * @brief printErrorMessages
     * Prints all recorded messages to the debug stream
     */
    void printErrorMessages();

private:
    /* Container for error messages */
    QStringList queryErrors;
};

#endif // QUERYFORMATEXCEPTION_H
