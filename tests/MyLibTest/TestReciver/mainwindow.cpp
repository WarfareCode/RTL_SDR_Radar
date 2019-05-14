#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _device = QSharedPointer<IReciverDevice>(new RTL_SDR_Reciver());
    _device->openDevice();
    _dataController = new DataController(_device,QSharedPointer<IDemodulator>());
    _dataController->run();

    _series = new QLineSeries();
    _series->setUseOpenGL(true);
    _chart = new QtCharts::QChart();
    _chart->addSeries(_series);
    _chart->setTitle("Zoom in/out example");
    _chart->setAnimationOptions(QChart::SeriesAnimations);
    _chart->legend()->hide();
    _chart->createDefaultAxes();

    _chartView = new QChartView(_chart);
    _chartView->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(_chartView);
    resize(800, 600);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);

    connect(&_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    _timer.start(10);

    QLineSeries *series = dynamic_cast<QLineSeries*>(_chart->series().first());
    _chart->axisX(series)->setMin((double)-1.0e6);
    _chart->axisX(series)->setMax((double)1.0e6);
    _chart->axisY(series)->setMax(0);
    _chart->axisY(series)->setMin(-180);

    inbuf = (complex*)fftw_malloc(sizeof(fftw_complex)*N);
    outbuf = (complex*)fftw_malloc(sizeof(fftw_complex)*N);
    plan = fftw_plan_dft_1d(N, (fftw_complex*)inbuf, (fftw_complex*)outbuf,
                            FFTW_FORWARD, FFTW_MEASURE);
}

MainWindow::~MainWindow()
{
    _timer.stop();
    _chart->removeSeries(_series);
    delete  _series;
    delete _chart;
    delete  _chartView;

    _device->closeDevice();
    _device->closeDevice();
    delete ui;

    fftw_destroy_plan(plan);
    fftw_free(inbuf);
    fftw_free(outbuf);
}



void MainWindow::timeout()
{
    static double freq = 4.09e9;


    double a0 = 0.35875;
    double a1 = 0.48829;
    double a2 = 0.14128;
    double a3 = 0.01168;

     _dataController->getDataToChart();
    QVector<uint8_t> dataVector = _device->getDataBlock(N*2);

    //qDebug()<<dataVector.size();

    memset(outbuf,0,N);
    int fft_pointer = 0;
    for(int i = 0; i < N; i+=2)
    {
        //The magic aligment happens here: we have to change the phase of each next complex sample
        //by pi - this means that even numbered samples stay the same while odd numbered samples
        //get multiplied by -1 (thus rotated by pi in complex plane).
        //This gets us output spectrum shifted by half its size - just what we need to get the output right.
        const double multiplier = (fft_pointer % 2 == 0 ? 1 : -1);
        complex bfr_val(dataVector[i], dataVector[i+1]);
        inbuf[fft_pointer] = (bfr_val - complex(127.0, 127.0)) * multiplier;
//        inbuf[fft_pointer].real( inbuf[fft_pointer].real() * (a0-a1*cos(2.0*M_PI*i/N)+a2*cos(4.0*M_PI*i/N)-a3*cos(6.0*M_PI*i/N)));
//        inbuf[fft_pointer].imag( inbuf[fft_pointer].imag() * 0);


//        inbuf[fft_pointer].real(cos( 3* 2*M_PI*fft_pointer/N));
//        inbuf[fft_pointer].imag(0);

        fft_pointer++;
    }

    fftw_execute(plan);

    double meanRe = 0.0, meanIm =0;
    complex z[N*2];

//    for (int i = 0; i < N; i++)
//    {
//        meanRe = inbuf[i].real();
//        meanIm = inbuf[i].imag();
//    }
//    meanRe /= N;
//    meanIm /= N;


//    double disp = 0.0;
//    for( int i = 0; i < N; i++ )
//    {
//        z[i] = inbuf[i].real() - meanRe;
//        disp += ( inbuf[i].real() - meanRe ) * ( inbuf[i].real() - meanRe );
//    }
//    disp /= ( N - 1 );

    if(_chart->series().isEmpty())
        return;

    QLineSeries *series = dynamic_cast<QLineSeries*>(_chart->series().first());
    if (series == nullptr)
        return;
    series->clear();

    double yAxisMax = 0.0, yAxisMin = 0.0;
    int xAxis = N;

    for (int i = 0, j = 0; i < N; i++, j+=2)
    {
//      float v =  outbuf[i].real()*outbuf[i].real();
//      float v1=  outbuf[i].imag()*outbuf[i].imag();

     // qDebug()<<v*v1 <<i;
      //QPointF p((qreal) i,v*v1);
      double re2 = outbuf[i].real() * outbuf[i].real();
      double im2 = outbuf[i].imag() * outbuf[i].imag();

      double result = 10 * log10(sqrt( re2 + im2)/pow(2,7) / N);
      //   double freq = (ND*(double)i/N*fs-fs/2.0)/1.e6;

      QPointF p(((qreal) i/N * 2.0e6 - 1e6),result);
        *series << p;
//        yAxisMax = qMax(yAxisMax,p.ry());
//        yAxisMin = qMin(yAxisMin,p.ry());
    }
    _chartView->repaint();

}


