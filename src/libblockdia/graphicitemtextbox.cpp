#include "graphicitemtextbox.h"

#include <QColor>
#include <QBrush>

libblockdia::GraphicItemTextBox::GraphicItemTextBox(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    this->text = "";
    this->minWidth = 0;
    this->font = QFont();
    this->padding = 5;
    this->bgColor = QColor("#fdd");
    this->algn = Align::Center;
    this->calculateDimensions();
    this->_isMouseHovered = false;

    // configurations
    this->setAcceptHoverEvents(true);
}

QRectF libblockdia::GraphicItemTextBox::boundingRect() const
{
    return this->currentBoundingRect;
}

void libblockdia::GraphicItemTextBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw box
    painter->fillRect(this->currentBoundingRect, QBrush((this->_isMouseHovered) ? QColor("#444") : this->bgColor));
    painter->setPen(QColor(Qt::black));
    painter->drawRect(this->currentBoundingRect);

    // calculate text y position
    QFontMetrics fm = QFontMetrics(this->font);
    qreal textY = - fm.descent() + fm.height() / 2.0;

    // calculate text x position
    qreal textX = 0.0;
    if (this->algn == Align::Left) {
        textX = -this->boundingRect().width()/2.0 + this->padding;
    } else if (this->algn == Align::Center) {
        textX = - this->_actualNeededWidth / 2.0 + this->padding;
    } else if (this->algn == Align::Right) {
        textX = this->boundingRect().width()/2 - this->_actualNeededWidth + this->padding;
    }

    // draw text
    painter->setFont(this->font);
    painter->setPen((this->_isMouseHovered) ? QColor(Qt::white) : QColor(Qt::black));
    painter->drawText(textX, textY, this->text);
}

void libblockdia::GraphicItemTextBox::updateData(const QString &text, Align align)
{
    this->prepareGeometryChange();
    this->text = text;
    this->algn = align;
    this->calculateDimensions();
}

qreal libblockdia::GraphicItemTextBox::actualNeededWidth()
{
    return this->_actualNeededWidth;
}

qreal libblockdia::GraphicItemTextBox::actualNeededHeight()
{
    return this->_actaulNeededHeight;
}

void libblockdia::GraphicItemTextBox::setMinWidth(qreal minWidth)
{
    this->prepareGeometryChange();
    this->minWidth = minWidth;
    this->calculateDimensions();
}

void libblockdia::GraphicItemTextBox::setBgColor(QColor bgColor)
{
    this->prepareGeometryChange();
    this->bgColor = bgColor;
}

bool libblockdia::GraphicItemTextBox::isMouseHovered()
{
    return this->_isMouseHovered;
}

void libblockdia::GraphicItemTextBox::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (this->isMouseHoverable) {
        this->prepareGeometryChange();
        this->_isMouseHovered = true;
    }
}

void libblockdia::GraphicItemTextBox::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (this->isMouseHoverable) {
        this->prepareGeometryChange();
        this->_isMouseHovered = false;
    }
}

void libblockdia::GraphicItemTextBox::calculateDimensions()
{
    // calculate new text size
    QFontMetrics fm = QFontMetrics(this->font);
    this->_actualNeededWidth = 2.0 * this->padding + fm.width(this->text);
    this->_actaulNeededHeight = 2.0 * this->padding + fm.height();

    // calculate new bounding rect
    qreal width = (this->minWidth > this->_actualNeededWidth) ? this->minWidth : this->_actualNeededWidth;
    qreal height = this->_actaulNeededHeight;
    this->currentBoundingRect = QRectF ( - width/2.0, - height/2.0, width, height);
}
