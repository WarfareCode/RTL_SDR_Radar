#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <QString>
#include <QFile>

#include "IObject.h"

class GraphicsObject : public QGraphicsObject
{
    Q_OBJECT

    double _rtAngle = 0;
    bool _isExistIcon = false;

    virtual void drawObjectIcon();
    //загрузка иконки объекта
    void loadPixmap();

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
public:
    explicit GraphicsObject();
    virtual ~GraphicsObject() override;

    void setRotateAngle(double rta);

 protected:

    QImage img;
    int _sizeIcon = 20;
    QColor _colorIcon =  QColor(0,250,0);
    bool _drawId = false;
    bool _drawName = false;
    bool _isSelect = false;
    QPointF _screenPos = QPointF(-10,-10);

    //задаем область отрисовки
    QRectF boundingRect() const override;
    //метод рисования
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *widget = 0) override;

    void checkIconState();

    void resetIconState();
};

#endif // GRAPHICSOBJECT_H
