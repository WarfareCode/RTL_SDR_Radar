#ifndef GRAPHICSWIDGET_H
#define GRAPHICSWIDGET_H

#include "graphicswidget_global.h"

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDebug>
#include <QThread>
#include <QGLWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QTimer>
#include <math.h>

#include "IPoolObject.h"
#include "IObject.h"
#include "../Subject/Subject.h"
#include "IMapController.h"
#include "coord/Position.h"
#include "../Carrier/ServiceLocator.h"


class GRAPHICSWIDGETSHARED_EXPORT GraphicsWidget: public QGraphicsView, public IObserver
{
    Q_OBJECT
public:

    explicit GraphicsWidget(double w = 600,
                            QSharedPointer<IPoolObject> poolObject = QSharedPointer<IPoolObject>(),
                            QWidget *parent = 0);
    ~GraphicsWidget() override;

    void setMapContoller(QSharedPointer<IMapController> ptr);

    void subscribe(QSharedPointer<IPoolObject> poolObject);
    void unsubscribe() override;
    void update(Subject* sub) override;


private:
    QSharedPointer<IPoolObject> _ptrPoolObject;
    QSharedPointer<IMapController> _ptrMapController;


    QTimer timer;
    QTimer _timer;
    //графическая сцена
    QGraphicsScene * _scene;

    double _widthWidget;
    double _heightWidget;
    //координата центра
    QPointF  _screenCenter;

    QPointF _fixCursorCoord;
    QPointF _cursorCoord;

    // масштаб
    int  _mapZoom;
    //пиксмап для карты
    QPixmap _pxmMap;
    QConicalGradient gradient;
    //пиксмапы для курсоров
    QPixmap _pxmCursor;
    QPixmap _pxmSelect;

    //рисовать инфу и легенду
    bool _isDrawInfo = true;
    //ночь или день
    bool _isNight = false;
    //рисовать карту
    bool _isDrawMap = false;
    //флаг фиксации
    bool _fixCursor = false;

    const int grad = 360;
    int32_t _cource = 0;
    uint16_t _sectorSize = 45;

    bool _updateInSector = true;

    const double _textBorder = 28;
    double _distToBorderMap;

    const QColor _clrGreen    =  QColor(0,128,0);
    const QColor _clrRed        = QColor(250,0,0);
    const QColor _clrBlue       = QColor(0,204,255);
    const QColor _clrYellow     = QColor(255,255,0);
    const QColor _clrWhite     = QColor(250,250,250);
    const QColor _clrGrey      = QColor(150,150,150);

    QVector<double > _vHiddenObject;

    const QSize _cursorSize = QSize(45,45);
    void initWidget(const QRect &size, bool enableOpenGL = false);
    void initCirsor();
    void changeCursorType(bool enableSystem);

    //фон
    void drawBackground ( QPainter * painter, const QRectF & rect ) override;
    //передний план
    void drawForeground(QPainter *painter, const QRectF &rect) override;


//    //узнать масштаб в километрах
//    double getDistanceRadarScale();
//    //рассчет дистанции до объекта
//    double getDistanceObject(const Position &pos);

    //события мыши
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;


//    // из координаты в экранную точку
//    QPointF geoToScreen(const Position &coord);
//    // Преобразования из точки экрана в координаты
//    Position screenToGeo(QPointF view);
//    void recalcCoordGraphObj();


protected:
    void resizeEvent(QResizeEvent *event) override;
    //лимб
    virtual void drawDotCicleWithLabel(QPainter *p, qreal rad);
    //центральная точка
    virtual void drawCarrier(QPainter *p);
    virtual void drawHiddenObject(QPainter*p);
    //информация
    virtual void drawInfo(QPainter *p);
    virtual void drawText(QPainter *p, double X, double Y, const QString &str, bool drawBorder = false);
    //координаты
    virtual void printCoord(QPainter *p);

    virtual void updateObjectOnScene(QSharedPointer<IObject> &object);
    virtual void recalculateCoordObjects();
    virtual QStringList getDataForTable(IObject *object);


private slots:
    void timeout();
    void slotUpdateData();
    //public slots:

    //    // +,- масштаба
    //    void RadarScalePlus();
    //    void RadarScaleMinus();

    //    //ночь-день
    //    void setNight(bool value);
    //    //обновление сцены при загрузке карты
    //    void updateScene();
signals:
    void signalDataToTable(QStringList list);
    void signalUpdateData();
};


#endif // GRAPHICSWIDGET_H
