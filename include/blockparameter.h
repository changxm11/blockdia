#ifndef BDPARAMETER_H
#define BDPARAMETER_H

#include "libglobals.h"

#include <QObject>
#include <QString>
#include <QXmlStreamReader>

namespace libblockdia {

// forward declarations
class Block;

/**
 * @brief Common base class (interface) for paramerters.
 */
class LIBBLOCKDIASHARED_EXPORT BlockParameter : public QObject
{
    Q_OBJECT

public:

    /**
     * @details Constructing a parameter
     * @param name The name for the parameter
     * @param parent The parent Block for the parameter (the parameter is automatically attached to the Block)
     */
    explicit BlockParameter(const QString &name, QObject *parent = 0);

    /**
     * @details The name of the parameter
     * It is recommended, that every parameter within
     * has a unique name within a certain block.
     * @return The name of the parameter.
     */
    QString name();

    /**
     * @see Parameter::name()
     * @param name Assigning a new name for the parameter
     */
    void setName(QString name);

    /**
     * @return Parameters can be defined as public or private.
     */
    bool isPublic();

    /**
     * @param isPublic Defines if this parameter is public.
     */
    void setPublic(bool isPublic);

    /**
     * @details Every parameter must have a default value.
     * A derived class must implement the default value.
     * @return The default value as string representation
     */
    virtual QString strDefaultValue() = 0;

    /**
     * @param value Setting the default value from a string
     * @return False if the value could not be set
     */
    virtual bool setDefaultValue(QString value) = 0;

    /**
     * @details This must be implemented by every derived class.
     * @return The current value as a QString
     */
    virtual QString strValue() = 0;

    /**
     * @param value Setting a new value from string
     * @return False if the value is outside the allowed range.
     */
    virtual bool setValue(QString value) = 0;

    /**
     * @details An information about the allowed values for this parameter.
     * This is used as informational message.
     * For example enums can return a string that lists all available enum items,
     * or integers can return a string with the allowed range (min..max).
     * @return An information about the allowed values for this parameter.
     */
    virtual QString allowedValues() = 0;

    /**
     * @details Parsing a XML stram for parameters
     * The XML stream is parsed from the current element for an parameter definition.
     * In every case the XML parser is set to after the current element (parent or next child).
     * @param xml The current xml parser
     * @param parent The parent Block object
     */
    static void importBlockDef(QXmlStreamReader *xml, QObject *parent);

    /**
     * @details This function imports parameter specific data from an xml stream.
     * It must be implemented by every subclass.
     * This is automatically called during importBlockDef()
     * @param xml The current xml parser
     * @return True if parsing was successful
     */
    virtual bool importParamDef(QXmlStreamReader *xml) = 0;

    /**
     * @details Exporting to an xml stream
     * @param xml The current xml writer
     * @return True on success
     */
    bool exportBlockDef(QXmlStreamWriter *xml);

    /**
     * @details This function export parameter specific data into an xml file.
     * It must be implemented by every subclass.
     * This is automatically called during exportBlockDef()
     * @param xml The current xml writer
     * @return True on success
     */
    virtual bool exportParamDef(QXmlStreamWriter *xml) = 0;


signals:
    /**
     * @details This signal is emitted whenever a value has changed.
     */
    void somethingHasChanged();

private:
    QString _name;
    bool _isPublic;
};

} // namespace bd

#endif // BDPARAMETER_H
