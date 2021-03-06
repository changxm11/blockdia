#include "blockparameterenum.h"
#include <QDebug>
#include <limits.h>

libblockdia::BlockParameterEnum::BlockParameterEnum(const QString &name, QObject *parent) : BlockParameter(name, parent)
{
    this->_value = "";
    this->_defaultValue = this->_value;
}

QString libblockdia::BlockParameterEnum::strDefaultValue()
{
    return this->_defaultValue;
}

bool libblockdia::BlockParameterEnum::setDefaultValue(QString value)
{
    if (this->_enumItems.contains(value)) {
        this->_defaultValue = value;
        return true;
    } else {
        return false;
    }
}

bool libblockdia::BlockParameterEnum::setValue(QString value)
{
    if (this->_enumItems.contains(value)) {
        this->_value= value;
        return true;
    } else {
        return false;
    }
}

QString libblockdia::BlockParameterEnum::strValue()
{
    return this->_value;
}

QString libblockdia::BlockParameterEnum::allowedValues()
{
    QString values = "";
    for (int i=0; i < this->_enumItems.size(); ++i) {
        if (values.length() > 0) values+= ", ";
        values += this->_enumItems.at(i);
    }

    return values;
}

bool libblockdia::BlockParameterEnum::addEnumItem(const QString &item)
{
    if (this->_enumItems.contains(item)) {
        return false;
    } else {
        this->_enumItems.append(item);
        return true;
    }
}

QStringList libblockdia::BlockParameterEnum::enumItems()
{
    return this->_enumItems;
}

bool libblockdia::BlockParameterEnum::setEnumItems(QStringList items)
{
    this->_enumItems = items;
    if (!this->_enumItems.contains(this->_defaultValue)) {
        if (this->_enumItems.size() > 0) this->_defaultValue = this->_enumItems[0];
        else this->_defaultValue = "";
    }
    return true;
}

bool libblockdia::BlockParameterEnum::importParamDef(QXmlStreamReader *xml)
{
    while (xml->readNextStartElement()) {
        if (xml->name() == "EnumItems") {

                // read enum items
                while (xml->readNextStartElement()) {

                    // append new item
                    if (xml->name() == "Item") {
                        this->addEnumItem(xml->attributes().value("name").toString());
                    } else {
                        qWarning() << "ERROR Parsing XML: unknown element (at line" << xml->lineNumber() << ")";
                    }

                    xml->skipCurrentElement();
                }

        } else {
            qWarning() << "ERROR Parsing XML: unknown element (at line" << xml->lineNumber() << ")";
            xml->skipCurrentElement();
        }
    }

    return xml->hasError();
}

bool libblockdia::BlockParameterEnum::exportParamDef(QXmlStreamWriter *xml)
{
    xml->writeStartElement("EnumItems");
    for (int i=0; i < this->enumItems().size(); ++i) {
        xml->writeStartElement("Item");
        xml->writeAttribute("name", this->enumItems().at(i));
        xml->writeEndElement();
    }
    xml->writeEndElement();
    return xml->hasError();
}
