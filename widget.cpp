#include "widget.h"
#include "ui_widget.h"

#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /* create thread and worker */
    thread = new QThread;
    worker = new CameraWorker;
    worker->moveToThread(thread);

    /* connection */
    // shortcut for quiting
    QShortcut *closeKey = new QShortcut(QKeySequence("Ctrl+W"), this);
    connect(closeKey, &QShortcut::activated, this, &QWidget::close);
    // param and flag of edge detection
    connect(this, &Widget::SetEdgeParamSignal, worker, &CameraWorker::SetEdgeParamSlot);
    connect(ui->CbBoxEdgeMode, &QComboBox::currentTextChanged, this, &Widget::SetEdgeParam);
    connect(this, &Widget::SetEdgeFlagSignal, worker, &CameraWorker::SetEdgeFlagSlot);
    // update frame
    connect(worker, &CameraWorker::UpdateFrameSignal, this, &Widget::UpdateFrameSlot);
    // start worker
    connect(thread, &QThread::started, this, &Widget::StartWorker);
    connect(this, &Widget::StartWorkerSignal, worker, &CameraWorker::StartWorkerSlot);
    // stop worker
    connect(this, &Widget::StopWorkerSignal, worker, &CameraWorker::StopWorkerSlot);

    /* start camera thread */
    thread->start();
}

Widget::~Widget()
{
    worker->deleteLater();

    thread->quit();
    thread->wait();
    thread->deleteLater();

    delete ui;
}


void Widget::UpdateFrameSlot(QImage frame)
{
    ui->LbCameraVideo->setPixmap(QPixmap::fromImage(frame));
}


void Widget::StartWorker()
{
    SetEdgeParam(ui->CbBoxEdgeMode->currentText());
    emit StartWorkerSignal(ui->LbCameraVideo->size().height(),
                           ui->LbCameraVideo->size().width());
}


void Widget::SetEdgeParam(QString key)
{
    if (edgeParam.contains(key))
    {
        emit SetEdgeFlagSignal(true);
        emit SetEdgeParamSignal(edgeParam[key]);
    }
    else
    {
        emit SetEdgeFlagSignal(false);
    }
}
