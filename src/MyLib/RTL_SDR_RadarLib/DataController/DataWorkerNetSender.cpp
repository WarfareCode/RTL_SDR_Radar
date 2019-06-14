#include <sys/types.h>
#include <unistd.h>

#include "DataWorkerNetSender.h"
#include "NetworkWorker.h"

using namespace std::chrono;

DataWorkerNetSender::DataWorkerNetSender(QSharedPointer<IReciverDevice> dev,
                             QSharedPointer<IDemodulator> dem,
                             size_t dataSize) :
                                                DataWorker (dev,dem,dataSize)
{
    qDebug()<<"create DataWorkerNet";
}

DataWorkerNetSender::~DataWorkerNetSender()
{
    if(_net)
        _net->disconnect();
    _net.reset(nullptr);

    qDebug()<<"delete DataWorkerNet";
}

void DataWorkerNetSender::exec()
{
    _net = std::unique_ptr<INetworkWorker>(new NetworkWorker(DEFAULT_IP,
                                                            DEFAULT_PORT));
    _abort = false;

    _firstTimeBreakpoint = steady_clock::now();

    forever
    {
        if(_abort)
            break;

        if(_net && !_net->isConnected())
            _net->connect(DEFAULT_IP,DEFAULT_PORT,CONNECT_TIMEOUT);

        QMutexLocker lock(&_mutex);
        if(processData())
        {
            _secondTimeBreakpoint = steady_clock::now();

            int64_t value = duration_cast<std::chrono::milliseconds>(_secondTimeBreakpoint - _firstTimeBreakpoint).count();
            if(value > SEND_INTERVAL)
            {
                if(_net && _net->isConnected())
                    _net->writeDatagramm(_demod->getRawDump());

                _firstTimeBreakpoint = steady_clock::now();
            }
        }
    }

    qDebug()<<"terminate thread id" << QThread::currentThreadId();
    emit finished();
}
