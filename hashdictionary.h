#ifndef HASHDICTIONARY_H
#define HASHDICTIONARY_H

#include <QHash>
#include <QString>

/**
 * @brief The HashDictionary class
 * This class represents a Trie data structured, used for autocomplete.
 * In this project, this data structure is used to filter out attribute
 * names as user is writing the attribute name. Only attributes, whose
 * beginning matches the input will be left in the list.
 * The data structure is a tree, in which each next depth level is
 * corresponding to the next letter in a word. Each node holds a letter
 * at relative position in the word and hashmap that contains
 * possibilities for the next letter. This way, finding a word in a tree
 * requires exactly L (length of the word) O(1) look ups.
 */
class HashDictionary
{
public:
    /**
     * @brief HashDictionary
     * Dictionary is created empty.
     */
    HashDictionary();

    /**
     * @brief insertWord - adds a new word to the dictionary.
     * It travels down the tree until letters of the new word match some
     * already exisiting path. If leaf node is reached, but word still has
     * letters left, it creates a new node at the next depth level to create
     * path for this word.
     * @param word - word to be inserted in the dictionary.
     */
    void insertWord(QString& word);

    /**
     * @brief findTree
     * When user provides part of the word to serach for matches, we travel down
     * the tree according to word letters. Returns the node to which we arrive
     * when all leters of the given word part are used.
     * @param wordPart - part of the word
     * @return - pointer to the node, which contains all the words that start with
     *           the given word part.
     */
    HashDictionary* findTree(QString wordPart);

    /**
     * @brief findWords
     * Finds all the words in the dictionary that begin with the given word part.
     * Method finds the node, whose subtree contains paths for all the words that
     * begin with the given word part and then traverses that tree to find all
     * known matching words
     * @param s - beginning of the word
     * @return - list of all words that begin with given string
     */
    QStringList findWords(QString s);

    /**
     * @brief traverseDictionary
     * Traverses the whole dictionary, returning all the words that
     * dictionary contains. All returned results will be prefixed by
     * the provided string.
     * @param s - prefix for all returned strings. If s is empty,
     *            then all returned strings will be 'original'
     * @return - list of all the words that this dictionary contains
     */
    QStringList traverseDictionary(QString s);

    /**
     * @brief GetRandomString
     * Generates random alphanumeric string of given length
     * @param length - length of the string to generate
     * @return -random alphanumeric string of given length
     */
    QString GetRandomString(int length);

private:
    /* Pointer to the root node of the tree */
    QHash<QChar, HashDictionary*>* mainDict;
};

#endif // HASHDICTIONARY_H
