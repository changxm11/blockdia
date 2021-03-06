#include "block.h"

#include <QDebug>
#include <QMetaClassInfo>
#include <QObjectList>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

libblockdia::Block::Block(QObject *parent) : QObject(parent)
{
    // set default params
    this->_TypeId       = "";
    this->_TypeName     = "";
    this->_InstanceId   = "";
    this->_InstanceName = "";
    this->_Color        = QColor("#fff");
    this->giBlock       = new GraphicItemBlock(this);
    connect(this, SIGNAL(signalSomethingChanged(libblockdia::Block*)), this, SLOT(slotUpdateGraphicItem()));
}



QString libblockdia::Block::typeId()
{
    return this->_TypeId;
}

void libblockdia::Block::setTypeId(const QString &id)
{
    if (id != this->_TypeId) {
        this->_TypeId = id;
        emit signalSomethingChanged(this);
    }
}

QString libblockdia::Block::typeName()
{
    return this->_TypeName;
}

void libblockdia::Block::setTypeName(const QString &name)
{
    if (name != this->_TypeName) {
        this->_TypeName = name;
        emit signalSomethingChanged(this);
    }
}


QString libblockdia::Block::instanceId()
{
    return this->_InstanceId;
}

void libblockdia::Block::setInstanceId(const QString &id)
{
    if (id != this->_InstanceId) {
        this->_InstanceId = id;
        emit signalSomethingChanged(this);
    }
}

QString libblockdia::Block::instanceName()
{
    return this->_InstanceName;
}

void libblockdia::Block::setInstanceName(const QString &name)
{
    if (name != this->_InstanceName) {
        this->_InstanceName = name;
        emit signalSomethingChanged(this);
    }
}

QColor libblockdia::Block::color()
{
    return this->_Color;
}

void libblockdia::Block::setColor(QColor color)
{
    this->_Color = color;
}

QList<libblockdia::BlockParameter *> libblockdia::Block::getParameters()
{
    return this->parametersList;
}

libblockdia::BlockParameter *libblockdia::Block::getParameter(const QString name)
{
    BlockParameter *ret = Q_NULLPTR;

    // find parameter by name
    for (int i = 0; i < this->parametersList.size(); ++i) {
        if (this->parametersList.at(i)->name() == name) {
            ret = this->parametersList.at(i);
            break;
        }
    }

    return ret;
}

QList<libblockdia::BlockInput *> libblockdia::Block::getInputs()
{
    return this->inputsList;
}

libblockdia::BlockInput *libblockdia::Block::getInput(const QString name)
{
    BlockInput *ret = Q_NULLPTR;

    // find input by name
    for (int i = 0; i < this->inputsList.size(); ++i) {
        if (this->inputsList.at(i)->name() == name) {
            ret = this->inputsList.at(i);
            break;
        }
    }

    return ret;
}

QList<libblockdia::BlockOutput *> libblockdia::Block::getOutputs()
{
    return this->outputsList;
}

libblockdia::BlockOutput *libblockdia::Block::getOutput(const QString name)
{
    BlockOutput *ret = NULL;

    // find input by name
    for (int i = 0; i < this->outputsList.size(); ++i) {
        if (this->outputsList.at(i)->name() == name) {
            ret = this->outputsList.at(i);
            break;
        }
    }

    return ret;
}

QGraphicsItem *libblockdia::Block::getGraphicsItem()
{
    return this->giBlock;
}

libblockdia::Block *libblockdia::Block::parseBlockDef(QIODevice *dev, libblockdia::Block *block)
{
    QXmlStreamReader xml(dev);

    while (!xml.atEnd()) {

        if (xml.readNextStartElement()) {

            // no block definition found
            if (xml.name() != "BlockDef") {
                qWarning() << "parseBlockDef: unknown root element:" << xml.name();
                xml.skipCurrentElement();
            } else {

                // parse different versions
                if (xml.attributes().value("version") == "1") {
                    if (!block) block = new Block();
                    parseBlockDefVersion1(&xml, block);
                    break;
                }

                // unknwon version
                else {
                    qWarning() << "parseBlockDef: unsupported version:" << xml.attributes().value("version");
                    xml.skipCurrentElement();
                }
            }
        }
    }

    return block;
}

bool libblockdia::Block::exportBlockDef(QIODevice *dev)
{
    QXmlStreamWriter xml(dev);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    // start block element
    xml.writeStartElement("BlockDef");
    xml.writeAttribute("version", "1");

        // type name
        xml.writeTextElement("TypeName", this->typeName());

        // type id
        xml.writeTextElement("TypeId", this->typeId());

        // color
        xml.writeTextElement("Color", this->color().name());

        // parameter
        xml.writeStartElement("Parameters");
        for (int i=0; i < this->parametersList.size(); ++i) {
            this->parametersList.at(i)->exportBlockDef(&xml);
        }
        xml.writeEndElement();

        // inputs
        xml.writeStartElement("Inputs");
        for (int i=0; i < this->inputsList.size(); ++i) {
            this->inputsList.at(i)->exportBlockDef(&xml);
        }
        xml.writeEndElement();

        // outputs
        xml.writeStartElement("Outputs");
        for (int i=0; i< this->outputsList.size(); ++i) {
            this->outputsList.at(i)->exportBlockDef(&xml);
        }
        xml.writeEndElement();

    // end block element
    xml.writeEndElement();

    xml.writeEndDocument();
    return false;
}

void libblockdia::Block::childEvent(QChildEvent *e)
{
    Q_UNUSED(e)

    // catch child add/delete event
    // delayed timer ensures that child is add/deleted completely
    // when calling the timer slot function
    QTimer::singleShot(0, this, SLOT(slotUpdateChildObjects()));
}

void libblockdia::Block::parseBlockDefVersion1(QXmlStreamReader *xml, libblockdia::Block *block)
{
    Q_ASSERT(xml->isStartElement() && xml->name() == "BlockDef");

    // read block definitions
    while (xml->readNextStartElement()) {

        // read type name
        if (xml->name() == "TypeName") {
            QString t = xml->readElementText(QXmlStreamReader::SkipChildElements);
            block->setTypeName(t);
        }

        // read type id
        else if (xml->name() == "TypeId") {
            QString t = xml->readElementText(QXmlStreamReader::SkipChildElements);
            block->setTypeId(t);
        }

        // read color
        else if (xml->name() == "Color") {
            QString t = xml->readElementText(QXmlStreamReader::SkipChildElements);
            block->setColor(QColor(t));
        }

        // check for inputs
        else if (xml->name() == "Inputs") {
            BlockInput::parseBlockDef(xml, block);
        }

        // check for outputs
        else if (xml->name() == "Outputs") {
            BlockOutput::parseBlockDef(xml, block);
        }

        // check for parameters
        else if (xml->name() == "Parameters") {
            BlockParameter::importBlockDef(xml, block);
        }

        // unknown tag
        else {
            xml->skipCurrentElement();
        }
    }
}

void libblockdia::Block::slotUpdateGraphicItem()
{
    this->giBlock->updateData();
}

void libblockdia::Block::slotUpdateChildObjects()
{
    bool emitSomethignChanged = false;
    QObjectList listChildren = this->children();


    // ------------------------------------------------------------------------
    //                               Find New Children
    // ------------------------------------------------------------------------

    for (int i=0; i < listChildren.size(); ++i) {
        // get liblock dia meta data
        QString childClass = listChildren.at(i)->metaObject()->className();
        QString childSuperClass = listChildren.at(i)->metaObject()->superClass()->className();

        // check for new parameters
        if (childSuperClass == "libblockdia::BlockParameter") {
            BlockParameter *child = (BlockParameter *) listChildren.at(i);
            if (this->parametersList.count(child) == 0) {
                this->parametersList.append(child);
                connect(child, SIGNAL(somethingHasChanged()), this, SLOT(slotUpdateGraphicItem()));
                emitSomethignChanged = true;
            }
        }

        // check for new inputs
        else if (childClass == "libblockdia::BlockInput") {
            BlockInput *child = (BlockInput *) listChildren.at(i);
            if (this->inputsList.count(child) == 0) {
                this->inputsList.append(child);
                connect(child, SIGNAL(somethingHasChanged()), this, SLOT(slotUpdateGraphicItem()));
                emitSomethignChanged = true;
            }
        }

        // check for new outputs
        else if (childClass == "libblockdia::BlockOutput") {
            BlockOutput *child = (BlockOutput *) listChildren.at(i);
            if (this->outputsList.count(child) == 0) {
                this->outputsList.append(child);
                connect(child, SIGNAL(somethingHasChanged()), this, SLOT(slotUpdateGraphicItem()));
                emitSomethignChanged = true;
            }
        }
    }



    // ------------------------------------------------------------------------
    //                           Delete Lost Children
    // ------------------------------------------------------------------------

    for (int i=0; i < this->parametersList.size(); ++i) {
        if (!listChildren.contains(this->parametersList.at(i))) {
            this->parametersList.removeAt(i);
            emitSomethignChanged = true;
        }
    }

    for (int i=0; i < this->inputsList.size(); ++i) {
        if (!listChildren.contains(this->inputsList.at(i))) {
            this->inputsList.removeAt(i);
            emitSomethignChanged = true;
        }
    }

    for (int i=0; i < this->outputsList.size(); ++i) {
        if (!listChildren.contains(this->outputsList.at(i))) {
            this->outputsList.removeAt(i);
            emitSomethignChanged = true;
        }
    }


    // emit singal
    if (emitSomethignChanged) {
        emit signalSomethingChanged(this);
    }
}
