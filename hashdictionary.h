#ifndef HASHDICTIONARY_H
#define HASHDICTIONARY_H

#include <QHash>
#include <QString>

class HashDictionary
{
public:
    HashDictionary();

    void insertWord(QString& word);
    HashDictionary* findTree(QString wordPart);
    QStringList findWords(QString s);
    QStringList traverseDictionary(QString s);
    QString GetRandomString();
private:
    QHash<QChar, HashDictionary*>* mainDict;
};

#endif // HASHDICTIONARY_H
