#include "GraphicsWidget.h"


GraphicsWidget::GraphicsWidget(double w,
                               QSharedPointer<IPoolObject> poolObject,
                               QWidget *parent):
    QGraphicsView(parent),
    _widthWidget(w),
    _heightWidget(w),
    _screenCenter( w / 2.0, w / 2.0),
    _pxmMap(static_cast<int>(_widthWidget),static_cast<int>(_widthWidget))
{
    subscribe(poolObject);
//    qDebug()<<"create GraphicsWidget" << QThread::currentThreadId();

    initWidget(QRect(0,
                     0,
                     static_cast<int>(_widthWidget),
                     static_cast<int>(_heightWidget)),
               true);
    initCirsor();

    //обновление сектора
    connect(&_timer,SIGNAL(timeout()),this,SLOT(timeout()));

    QObject::connect(this,
                     SIGNAL(signalUpdateData()),
                     this,
                     SLOT(slotUpdateData()));

}

GraphicsWidget::~GraphicsWidget()
{
    qDebug()<<" ~GraphicsWidget() -> start delete scene";
    if(_scene)
        for (auto &iter:_scene->items())
        {
            if(iter)
                _scene->removeItem(iter);
        }
    delete _scene;
    _scene = nullptr;
    qDebug()<<"~GraphicsWidget() -> clear and delete scene";

    unsubscribe();
}


void GraphicsWidget::setMapContoller(QSharedPointer<IMapController> ptr)
{
    _ptrMapController = ptr;
}

void GraphicsWidget::subscribe(QSharedPointer<IPoolObject> poolObject)
{
    if(poolObject.isNull())
    {
        qDebug()<<" MainWindow::setPoolObjectAndSubscribe -> try subscribe nullptr";
        return;
    }

    _ptrPoolObject = poolObject;

    Subject* sbj = dynamic_cast<Subject*>(_ptrPoolObject.data());

    if(sbj)
        sbj->Attach(this);
}

void GraphicsWidget::unsubscribe()
{
    qDebug()<<"unsubscribe GraphicsWidget";
    Subject* sbj = dynamic_cast<Subject*>(_ptrPoolObject.data());
    if(sbj)
        sbj->Deatach(this);
    sbj = nullptr;
    _ptrPoolObject.clear();
}


void GraphicsWidget::initWidget(const QRect &rect, bool enableOpenGL)
{
    this->setOptimizationFlags( QGraphicsView::DontClipPainter  |
                                QGraphicsView::DontSavePainterState |
                                QGraphicsView::DontAdjustForAntialiasing );

    this->setViewportUpdateMode( QGraphicsView::BoundingRectViewportUpdate );

    this->setGeometry(rect);

    //равнение на ... середину
    setAlignment(Qt::AlignCenter);

    //кэшируем фон
    setCacheMode(CacheBackground);

    if(enableOpenGL)
        setViewport(new QGLWidget);

    //как будет обновляться наша сцена
    //BoundingRectViewportUpdate - обновлять в области указаной пользователем
    setViewportUpdateMode(BoundingRectViewportUpdate);

    //сглаживание
    setRenderHint(QPainter::Antialiasing);

    //как позиционировать сцену во время трансформации
    setTransformationAnchor(AnchorViewCenter);

    //добавляем сцену
    _scene = new QGraphicsScene(this);

    //отслеживания события движения мыши-с кликом или без
    setMouseTracking (true);

    //не индексируем itemы, так будет быстрей работать
    _scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    //область рисование
    _scene->setSceneRect(0,0,this->width(),this->width());

    //установка сцены
    this->setScene(_scene);


    //убрать скролл
    this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    this->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

    _distToBorderMap = _scene->sceneRect().width()/2.0 - _textBorder ;

}

void GraphicsWidget::initCirsor()
{
    //нарисуем свой модный курсор зелёного цвета
    _pxmCursor = QPixmap(_cursorSize);
    _pxmCursor.fill(Qt::transparent);
    _pxmCursor.load(QString(":/icon/app/pxmCursor.png"));

    //нарисуем свой модный курсор красного цвета
    _pxmSelect = QPixmap(_cursorSize);
    _pxmSelect.fill(Qt::transparent);
    _pxmSelect.load(QString(":/icon/app/pxmSelect.png"));

    //нарисуем курсор
    changeCursorType(false);
    _fixCursor = false;
}

void GraphicsWidget::changeCursorType(bool enableSystem)
{
    if(enableSystem)
        //установим обычный курсор
        this->setCursor(Qt::ArrowCursor);
    else
        this->setCursor(QCursor(_pxmCursor));
}


void GraphicsWidget::update(Subject *sub)
{
    if((sub == nullptr) || (_ptrPoolObject == nullptr))
    {
        qDebug()<<"GraphicsWidget::update() :: nullptr detect" << sub << _ptrPoolObject;
        return;
    }
    IPoolObject* sbj = dynamic_cast<IPoolObject*>(sub);

    //надо убедится что указатель что у нас установлен равен указателю который пришел, вдруг это не то объект
    if(sbj == _ptrPoolObject)
        slotUpdateData();
}

void GraphicsWidget::updateObjectOnScene(QSharedPointer<IObject> &object)
{
    if(_ptrMapController.isNull())
        return;

    if(object.isNull())
        return;

    QGraphicsItem* graphItem = dynamic_cast<QGraphicsItem*>(object.data());

    if(graphItem == nullptr)
        return;

    if( object->getObjectState() == OBJECT_STATE::DELETE_OBJECT ||
            (!object->getInUse()))
    {
        if(scene()->items().contains(graphItem))
            _scene->removeItem(graphItem);
        return;
    }

    if(!object->isValidGeoCoord())
    {
        _scene->removeItem(graphItem);
        return;
    }

    if(!scene()->items().contains(graphItem))
    {
        graphItem->setFlag(QGraphicsItem::ItemIsSelectable);
        scene()->addItem(graphItem);
    }


    QPointF dot = {-10.0,-10.0};


    if(object->getDistance_KM() < (_ptrMapController->getDistanceRadarScale_KM()))
    {
        dot = _ptrMapController->geoToScreenCoordinates(object->getGeoCoord());

        graphItem->setPos(dot);
        graphItem->setOpacity(1);

        if(!graphItem->isVisible())
            graphItem->show();
    }
    else
    {
        _vHiddenObject.append(object->getAzimuth());
        graphItem->hide();
    }


    //если графический объект выбран текущим
    if(object->isSelectedObject() && _fixCursor)
    {
        _fixCursorCoord = dot;
        QStringList list = getDataForTable(dynamic_cast<IObject*>(object.data()));
        emit signalDataToTable(list);
    }
}

void GraphicsWidget::recalculateCoordObjects()
{
    _vHiddenObject.clear();
    if(_ptrPoolObject)
        for (auto &iter:_ptrPoolObject->values())
            updateObjectOnScene(iter);

    QGraphicsView::resetCachedContent();
    _scene->update();
}

QStringList GraphicsWidget::getDataForTable(IObject* object)
{
    QStringList list;
    if(!object)
        return list;

    list << QString::number(object->getId())
         << object->getDateTimeStart().toString("dd:MM:yy hh:mm:ss")
         << object->getDateTimeStop().toString("dd:MM:yy hh:mm:ss")
         << QString::number(object->getAzimuth())
         << QString::number(object->getElevation())
         << object->getObjectName()
         << QString::number(object->getDistance_KM())
         << QString::number(object->getGeoCoord().latitude())
         <<  QString::number(object->getGeoCoord().longitude());

    return list;
}

void GraphicsWidget::mouseMoveEvent(QMouseEvent *event)
{
    _cursorCoord = mapToScene(event->pos());

    //если рамка не зафиксирована и пользователь теребонькает курсором над объектами
    if(!_fixCursor)
    {
        QGraphicsItem * litem = nullptr;
        QTransform trans;
        //получаем значение элемента над которым находится перекрестие курсора
        litem = _scene->itemAt(mapToScene(event->pos()),trans);

        //установка флага текущего
        IObject* graphObject = dynamic_cast<IObject*> (litem);
        if(graphObject)
        {
            scene()->clearSelection();
            litem->setSelected(true);

            QStringList list = getDataForTable(dynamic_cast<IObject*>(litem));
            emit signalDataToTable(list);
        }
    }
}

void GraphicsWidget::mousePressEvent(QMouseEvent *event)
{
    //нажали ЛКМ
    if(event->button() != Qt::LeftButton)
        return;

    _fixCursor = !_fixCursor;

    if(_fixCursor)
    {
        QGraphicsItem * litem = nullptr;
        QTransform trans;
        //получаем значение элемента над которым находится перекрестие курсора
        litem = _scene->itemAt(mapToScene(event->pos().x(),event->pos().y()),trans);
        if(litem)
        {
            litem->setSelected(true);
            _fixCursorCoord = litem->pos();
        }
        else
            //для определения положения курсора
            _fixCursorCoord = mapToScene(event->pos());
    }
    changeCursorType(_fixCursor);
    _scene->update();//обновим всё
}

void GraphicsWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    //!!!!!!!!сюда вставить отрисовку карты
    //!

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QBrush(_clrGreen),1));

    //внешние точки, большие через 5 градусов
    double rad = _scene->sceneRect().width()/2 - _textBorder + 5;
    //рисуем большие точки и надписи
    drawDotCicleWithLabel(painter, rad);

}

void GraphicsWidget::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->setRenderHint(QPainter::Antialiasing);
    //если у нас зафиксирован курсор

    if(_fixCursor)
    {
        //рисуем свой курсор красного цвета
        painter->drawPixmap(static_cast<int32_t>(_fixCursorCoord.x()-_pxmSelect.width()/2),
                            static_cast<int32_t>(_fixCursorCoord.y()-_pxmSelect.height()/2),
                            _pxmSelect.width(),
                            _pxmSelect.height(),
                            _pxmSelect);
    }

    QSharedPointer<ICarrierClass> _ptrCarrier = ServiceLocator::getCarrier();


    drawCarrier(painter);

    //TODO: исправить расчет положения надписей
//    drawInfo(painter);
//    printCoord(painter);

    //qDebug()<< QThread::currentThreadId() <<"draw";

    if(_updateInSector)
    {

        // это сделано для перемещения карты.
        QPointF dot = _screenCenter;

        if(!_ptrMapController.isNull())
        {
            dot = _ptrMapController->geoToScreenCoordinates(_ptrCarrier->getGeoCoord());
        }

        //для отрисовки градиента, как на старых радарах
        QRect drawingRect(dot.x() - _distToBorderMap,
                          dot.y() - _distToBorderMap,
                          _distToBorderMap * 2,
                          _distToBorderMap * 2);

        //этот вариант проще
        //второй вариант - это один раз нарисовать градиент и вращать его
        //по производительности одинаково, но так меньше кода
        gradient.setCenter(drawingRect.center());
        gradient.setAngle(-_cource);
        gradient.setColorAt(0, QColor(0, 180, 0,100));
        gradient.setColorAt(0.2, QColor(0,0,0,0));

        painter->setPen(QPen(Qt::black));
        painter->setBrush(QBrush(Qt::black));
        painter->setBrush(QBrush(gradient));

        painter->drawEllipse(drawingRect);

        painter->setPen(QColor(0,220,0));

        painter->drawLine(drawingRect.center(),QPointF(ScreenConversions::polarToScreen(_screenCenter,
                                                                                        _cource + _sectorSize * 2,
                                                                                        _distToBorderMap)));
    }
}

void GraphicsWidget::resizeEvent(QResizeEvent *event)
{

    int h = event->size().height();
    int w = event->size().width();

    if( w < 400 || h < 400 )
        return;

    if(w > h)
        w = h;
    else
        h = w;
    const uint64_t dimension = static_cast<uint64_t>(sqrt(w * h));
    _scene->setSceneRect(0, 0, dimension, dimension);

    _widthWidget = _heightWidget = dimension;
    _distToBorderMap = _scene->sceneRect().width()/2.0 - _textBorder ;
    _screenCenter.setX(_scene->sceneRect().width()/2.0);
    _screenCenter.setY(_scene->sceneRect().width()/2.0);

    recalculateCoordObjects();
}


void GraphicsWidget::drawCarrier(QPainter *p)
{
    QSharedPointer<ICarrierClass> _ptrCarrier = ServiceLocator::getCarrier();
    // это сделано для перемещения карты.
    QPointF dot = _screenCenter;

    if(!_ptrMapController.isNull())
    {
        dot = _ptrMapController->geoToScreenCoordinates(_ptrCarrier->getGeoCoord());
    }
    p->save();
    p->setPen(QPen(QBrush(QColor(0,250,0)),5));
    p->drawPoint(dot);
    p->restore();
}


void GraphicsWidget::drawHiddenObject(QPainter *p)
{

    //кто-то за масштабом
    static const QPointF points[3] =
    {
        QPointF(6, 0.0),
        QPointF(0.0, 12.0),
        QPointF(12.0, 12.0)
    };

    for(auto & iter: _vHiddenObject)
    {
        SPolarCoord crd(iter,_distToBorderMap);

        QPointF dot = ScreenConversions::polarToScreen(
                    _screenCenter,
                    iter - 1,
                    _distToBorderMap);

        QPen pen(p->pen());
        p->setPen(QPen(QBrush(_clrGreen),2));
        p->save();
        p->translate(dot);

        p->rotate(crd.phi);

        p->setBrush(QColor(34,139,34));
        p->setPen(QColor(0,255,127));

        p->drawPolygon(points, 3);
        p->restore();
        p->setPen(pen);
    }
}

void GraphicsWidget::drawInfo(QPainter *p)
{
    p->save();
    QFont font = p->font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setStyleHint(QFont::Monospace,QFont::PreferAntialias);
    font.setPointSize(static_cast<int>(font.pointSize() * 1.1));
    font.setBold(true);
    p->setFont(font);

    QPen pen(p->pen());
    p->setPen(QPen(QBrush(_clrWhite),1));

    //Время
    double X = _scene->sceneRect().topRight().x();
    double Y = _scene->sceneRect().topRight().y();

    QString caption = QString::fromLocal8Bit("    СИСТЕМНОЕ ВРЕМЯ    ");
    drawText(p, X, Y ,caption,true);

    caption = QString::fromLocal8Bit("%1")
            .arg(QDateTime::currentDateTimeUtc().toString("dd.MM.yyyy hh:mm:ss UTC"));

    drawText(p, X , Y + QFontMetrics(font.family()).ascent() + 5 ,caption);

    //курс,скорость
    X = _scene->sceneRect().topLeft().x();
    Y = _scene->sceneRect().topLeft().y();

    caption = QString::fromLocal8Bit("    НАВИГАЦИЯ    ");
    drawText(p, X , Y , caption,true);

    QString longitude, latitude;

    Position pos;

    QSharedPointer<ICarrierClass> _ptrCarrier = ServiceLocator::getCarrier();
    if(_ptrCarrier)
        pos = _ptrCarrier->getGeoCoord();

    Conversions::geoCoordToString(pos,longitude,latitude);
    longitude.prepend(trUtf8("Д = "));
    latitude.prepend(trUtf8("Ш = "));
    drawText(p, X + 10 , Y + QFontMetrics(font.family()).ascent() + 5 , latitude,false);
    drawText(p, X + 10 , Y + 2*QFontMetrics(font.family()).ascent() + 5 , longitude,false);

    p->setPen(pen);
    p->restore();
}

void GraphicsWidget::drawText(QPainter *p, double X, double Y, const QString &str, bool drawBorder)
{
    QFont font = p->font();
    int captionWidth = QFontMetrics(font.family()).width(str);
    int captionHeight = QFontMetrics(font.family()).ascent() + 2;

    if(X > _scene->width()/2)
        X -= captionWidth;

    if(Y > _scene->width()/2)
        Y -= captionHeight;

    QRectF rect(X,
                Y + QFontMetrics(font.family()).ascent() + 5.0,
                captionWidth,
                captionHeight);
    if(drawBorder)
        p->drawRect(rect);
    p->drawText(rect,Qt::AlignCenter,str);
}

void GraphicsWidget::printCoord(QPainter *p)
{

    QFont font = p->font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setStyleHint(QFont::Monospace,QFont::PreferAntialias);
    font.setBold(true);
    p->setFont(font);

    QPen pen(p->pen());
    pen.setCosmetic(true);
    p->setPen(QPen(QBrush(_clrWhite),2));

    double X = _scene->sceneRect().bottomLeft().x();
    double Y = _scene->sceneRect().bottomLeft().y();

    QString caption(QString("X = %1 , Y = %2 ").arg(_cursorCoord.x()).arg(_cursorCoord.y()));
    drawText(p, X, Y - 4 * QFontMetrics(font.family()).ascent() + 5,caption);


    //эти данные должен давать модуль картографии

    //    p->drawText(cX,cY-2*h,w,h,Qt::AlignLeft|Qt::AlignVCenter,
    //                rus("M = %1 км").
    //                arg(int(getDistanceRadarScale())));

    //    SPolarCoord plrCrs;
    //    plrCrs = coordConv->screenToRealPolar(_cursorCoord);
    //    p->drawText(cX,cY-4*h,w,h,Qt::AlignLeft|Qt::AlignVCenter,
    //                rus("П = %1 ")
    //                .arg(QString::number(plrCrs.phi,'g',3)));


    //    double dist = coordConv->getDistance(CORVETTE.getGeoCoord(),coordConv->screenToGeo(_fixCursorCoord));
    //    if(dist > getDistanceRadarScale())
    //        dist = 0;
    //    p->drawText(cX,cY-3*h,w,h,Qt::AlignLeft|Qt::AlignVCenter,
    //                rus("Д = %1")
    //                .arg(QString::number((dist),'g',5)));
}





void GraphicsWidget::timeout()
{
    _cource += 2;
    _scene->update();
}

void GraphicsWidget::slotUpdateData()
{
    if(!_ptrPoolObject->tryLockPool())
        return;

    for (auto &iter: _ptrPoolObject->values())
    {
        if(iter.isNull())
            continue;
        updateObjectOnScene(iter);
    }
    _ptrPoolObject->unlockPool();

    //qDebug()<<"GraphicsWidget::update()->slotUpdateData();" <<_scene->items().count();
    _scene->update();
}

// функция рисования точек с надписями
void GraphicsWidget::drawDotCicleWithLabel(QPainter *p, const qreal rad)
{
    int w = 22 , h = 20;
    p->save();
    //с шагом grad
    for (int i = 0; i < grad; ++i)
    {
        p->setPen(QPen(QBrush(_clrGreen), 2));
        if((i % 5) == 0)
            p->setPen(QPen(QBrush(_clrGreen), 5));
        //вычисляем координату точки и рисуем её
        QPointF dot = ScreenConversions::polarToScreen(_screenCenter, i, rad);

        p->drawPoint(dot);

        //здесь делаем надпись каждые 10 градусов
        if((i % 10) == 0)
        {
            p->setPen(QPen(QBrush(_clrWhite), 1));

            if((i >= 0) && (i <= 90))
                p->drawText(QRectF(dot.x(), dot.y() - h, w, h),
                            Qt::AlignCenter,
                            QString::number(i));
            if((i > 90) && (i < 180))
                p->drawText(QRectF(dot.x(), dot.y(), w, h),
                            Qt::AlignCenter,
                            QString::number(i));
            if((i >= 180) && (i <= 270))
                p->drawText(QRectF(dot.x() - w - 2, dot.y(), w, h),
                            Qt::AlignCenter,
                            QString::number(i));
            if((i > 270))
                p->drawText(QRectF(dot.x() - h, dot.y() - h, w , h ),
                            Qt::AlignCenter,
                            QString::number(i));
        }
    }
    p->restore();
}
