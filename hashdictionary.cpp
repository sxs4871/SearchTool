#include "hashdictionary.h"
#include <QDebug>

HashDictionary::HashDictionary() {
    mainDict = new QHash<QChar, HashDictionary*>;
}

void HashDictionary::insertWord(QString& word) {
    if (word.size() == 0) {
        mainDict->insert('\0', 0);
        return;
    }
    QChar c = word.at(0);
    QHash<QChar, HashDictionary*>::iterator it = mainDict->find(c);
    HashDictionary* nextDict;
    if (it == mainDict->end()) {
        nextDict = new HashDictionary();
        mainDict->insert(c, nextDict);
    } else {
        nextDict = it.value();
    }
    QString remainingWord = word.remove(0, 1);
    nextDict->insertWord(remainingWord);
}

HashDictionary* HashDictionary::findTree(QString wordPart) {
    if (wordPart.size() == 0) {
        return this;
    }
    QChar c = wordPart.at(0);
    QHash<QChar, HashDictionary*>::iterator it = mainDict->find(c);
    if (it == mainDict->end()) {
        return new HashDictionary();
    } else {
        QString remainingWord = wordPart.remove(0, 1);
        return it.value()->findTree(remainingWord);
    }
}

QStringList HashDictionary::findWords(QString wordPart) {
    HashDictionary* matchingTree = findTree(wordPart);
    return matchingTree->traverseDictionary(wordPart);
}

QStringList HashDictionary::traverseDictionary(QString s) {
    QHash<QChar, HashDictionary*>::iterator it;
    QStringList list;
    for (it = mainDict->begin(); it != mainDict->end(); it++) {
        if (it.key() == '\0') {
            list.append(s);
        } else {
            QString ss = s + it.key();
            list = list + it.value()->traverseDictionary(ss);
        }
    }
    return list;
}

QString HashDictionary::GetRandomString(int length)
{
   QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

   QString randomString;
   for(int i = 0; i < length; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}
