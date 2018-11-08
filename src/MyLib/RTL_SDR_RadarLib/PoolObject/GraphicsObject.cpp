#include "GraphicsObject.h"

GraphicsObject::GraphicsObject()
{
    //перемещаем в необходимую точку
    setPos(_screenPos);
    //загружаем иконку
    loadPixmap();
}

GraphicsObject::~GraphicsObject()
{

}

void GraphicsObject::setRotateAngle(double rta)
{
    _rtAngle = rta;
}

QRectF GraphicsObject::boundingRect() const
{
    return QRectF( - _sizeIcon/2, - _sizeIcon/2,_sizeIcon,_sizeIcon);
}

void GraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter->save();

    if(_isExistIcon)
        painter->rotate(_rtAngle + 90);

    painter->drawImage(boundingRect().x(),boundingRect().y(),img);
    painter->restore();

    //выбран
    if(_isSelect)
    {
        painter->setPen(QPen(QBrush(QColor(250,0,0)),
                             2,
                             Qt::DashDotLine));

        painter->drawEllipse(boundingRect().center(),
                             boundingRect().left()-1,
                             boundingRect().top()-1);
    }
}

void GraphicsObject::drawObjectIcon()
{
    //qDebug()<<"GraphicsObject::drawObjectIcon()";
    _sizeIcon = 20;
    img = QImage(_sizeIcon,_sizeIcon,QImage::Format_ARGB32_Premultiplied);
    if(img.isNull())
        return;

    img.fill(0x0);

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing);

    qreal cX = img.width() / 2;
    qreal cY = img.width() / 2;
    qreal rad = _sizeIcon / 2;

    p.setPen(QPen(QBrush(_colorIcon), 2));

    QPointF c(cX,cY);
    p.drawEllipse(c, rad - 1, rad - 1);

    p.setPen(QPen(QBrush(_colorIcon), 2));

    QVector<QPointF> ln;

    //вершина молнии
    ln.append(QPointF(cX, cY - rad + 3.0));
    ln.append(QPointF(cX - rad / 3.0, cY + 2.0));

    //середина
    ln.append(QPointF(cX - rad / 3.0, cY + 2.0));
    ln.append(QPointF(cX + rad / 3.0, cY - 2.0));

    //конец
    ln.append(QPointF(cX + rad / 3.0,cY - 2.0));
    ln.append(QPointF(cX, cY + rad - 3.0));

    p.drawLines(ln);
}

void GraphicsObject::loadPixmap()
{
    QString str(":/icon/target/");

    QString name = "air";
    str = str + name + "/" + name + ".png";
    _sizeIcon = 25;
    if(QFile::exists(str))
    {
        QImage t_img = QImage(str);

        if(!t_img.isNull())
        {
            img = QImage(t_img.scaled(_sizeIcon,
                                      _sizeIcon,
                                      Qt::IgnoreAspectRatio,
                                      Qt::SmoothTransformation));
            if(!img.isNull())
            {
                _isExistIcon = true;
                return;
            }
        }
    }

    _isExistIcon = false;

    drawObjectIcon();
}


void GraphicsObject::resetIconState()
{
    _rtAngle = 0;
    _isExistIcon = false;
}



QVariant GraphicsObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene())
    {
        _isSelect = value.toBool();
        if(_isSelect)
        {
            setZValue(1);
            setScale(1.2);
        }
        else
        {
            setZValue(0);
            setScale(1);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
