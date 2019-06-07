#ifndef LED_H
#define LED_H

#include <QWidget>

class Led : public QWidget
{
Q_OBJECT

public:
    Led(QWidget *parent = nullptr);
    void setColor(QColor _color);

protected:
    QColor color;
    void paintEvent(QPaintEvent *event);
};

 #endif
