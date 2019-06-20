
#include <QtGui>

#include "led.h"

Led::Led(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(QSize(10,10));
    color = Qt::red;
}

void Led::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRect rect = QRect(1,1,width()-2,height()-2);
    QRadialGradient gradient = QRadialGradient(QPointF(rect.width()/2.7, rect.height()/2.7), rect.width());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(0.5, color);
    gradient.setColorAt(1, color);
    QBrush brush = QBrush(gradient);
    painter.setBrush(brush);
    painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine));
    painter.drawEllipse(rect);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect);
}

void Led::setColor(QColor _color)
{
    color = _color;
    update();
}
