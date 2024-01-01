#include "CameraWorker.h"
#include <QDebug>

CameraWorker::CameraWorker(QObject *parent) : QObject(parent)
    , nKernelSize(15)
    , nCannyThrd1(15)
    , nCannyThrd2(45)
    , bEnableEdgeDetect(false)
    , HEIGHT(480)
    , WIDTH(640)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CameraWorker::UpdateFrame);
}


CameraWorker::~CameraWorker()
{
    qInfo() << "[INFO] Camera worker finished";
}


void CameraWorker::StartWorkerSlot(const int height, const int width)
{
    HEIGHT = height;
    WIDTH = width;

    cap.open(0, cv::CAP_ANY);
    if (!cap.isOpened())
    {
        qCritical() << "[Error] Unable to open camera";
        return;
    }

    // start timer with interval of 33 ms (30 fps)
    timer->start(33);
}


void CameraWorker::StopWorkerSlot()
{
    timer->stop();
}


void CameraWorker::UpdateFrame()
{
    cv::Mat frame;
    cap.read(frame);
    if (frame.empty())
    {
        qCritical() << "[ERROR] Blank frame grabbed";
        return;
    }

    // resize and flip
    cv::resize(frame, frame, cv::Size(WIDTH, HEIGHT));
    cv::flip(frame, frame, 1);  // horizontal flip

    if (bEnableEdgeDetect)
    {
        frame = ProcessImage(frame);
    }

    // create QImage
    QImage Qframe;
    switch (frame.type())
    {
    case CV_8UC3:
        Qframe = QImage(frame.data,
                        WIDTH, HEIGHT,
                        static_cast<int>(frame.step),
                        QImage::Format_RGB888).rgbSwapped();
        break;

    case CV_8UC1:
        Qframe = QImage(frame.data,
                        WIDTH, HEIGHT,
                        static_cast<int>(frame.step),
                        QImage::Format_Grayscale8);
        break;

    default:
        qWarning() << "[WARNING] Invalid frame type";
        return;
    }

    emit UpdateFrameSignal(Qframe);
}


cv::Mat CameraWorker::ProcessImage(const cv::Mat &frame)
{
    cv::Mat frame_gray, frame_edge;

    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

    // canny edge detection
    cv::blur(frame_gray, frame_edge, cv::Size(nKernelSize, nKernelSize));
    cv::Canny(frame_gray, frame_edge, nCannyThrd1, nCannyThrd2);

    return frame_edge;
}


void CameraWorker::SetEdgeParamSlot(const QVector<int> &edgeParam)
{
    if (edgeParam.size() != 3)
    {
        qWarning() << "[WARNING] Invalid parameter format";
        return;
    }
    nKernelSize = edgeParam[0];
    nCannyThrd1 = edgeParam[1];
    nCannyThrd2 = edgeParam[2];
}


void CameraWorker::SetEdgeFlagSlot(bool bEnable)
{
    bEnableEdgeDetect = bEnable;
}
