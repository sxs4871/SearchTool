#ifndef AVU_H
#define AVU_H

#include <QString>

/**
 * @brief The AVU class
 * AVU class is a container class representing metadata
 * about objects in the iRODS system. AVU stands for
 * 'Attribute, Value, Unit'. AVU class holds this data triplet.
 */
class AVU
{
public:
    /* Methods */
    /**
     * @brief AVU - Constructor. Attribute must have a name,
     * value and units will be assumed optional.
     * @param _attrName - name of the attribute
     * @param _value - value of the attribute
     * @param _unit - units for the attribute
     */
    AVU(QString _attrName, QString _value = 0, QString _unit = 0);

    /**
     * @brief toString - Convenience methods for displaying
     * an AVU triplet nicely.
     * @return - String representation of this AVU triplet.
     */
    QString toString();

    /* Data accessors */
    QString getAttrName();
    QString getValue();
    QString getUnit();
    int getId() const;
    void setId(int id);

private:
    /* AVU data contrainers */
    QString attrName;
    QString value;
    QString unit;
    int id;

};

#endif // AVU_H
