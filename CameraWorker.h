#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <QObject>
#include <QImage>
#include <QTimer>

#include <opencv2/opencv.hpp>

class CameraWorker : public QObject
{
    Q_OBJECT
public:
    explicit CameraWorker(QObject *parent = nullptr);
    ~CameraWorker();

public slots:
    void StartWorkerSlot(const int height, const int width);
    void StopWorkerSlot();
    void SetEdgeParamSlot(const QVector<int> &edgeParam);
    void SetEdgeFlagSlot(bool bEnable);

private:
    cv::VideoCapture cap;
    int nKernelSize;
    int nCannyThrd1, nCannyThrd2;
    bool bEnableEdgeDetect;
    int HEIGHT, WIDTH;
    QTimer *timer;

    void UpdateFrame();
    cv::Mat ProcessImage(const cv::Mat &frame);

signals:
    void UpdateFrameSignal(QImage frame);
};

#endif // CAMERAWORKER_H
