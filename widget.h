#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QShortcut>
#include <QThread>
#include <QMap>
#include <QString>
#include <QImage>
#include <QPixmap>

#include "CameraWorker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void UpdateFrameSlot(QImage frame);

private:
    Ui::Widget *ui;
    QThread *thread;
    CameraWorker *worker;

    const QMap<QString, QVector<int>> edgeParam = {
        {QStringLiteral("Clean"), QVector<int>{60, 35, 175}},
        {QStringLiteral("Standard"), QVector<int>{45, 35, 100}},
        {QStringLiteral("Detail"), QVector<int>{30, 10, 30}}
    };

    void StartWorker();
    void SetEdgeParam(QString key);

signals:
    void StartWorkerSignal(int height, int width);
    void StopWorkerSignal();
    void SetEdgeParamSignal(QVector<int> edgeParam);
    void SetEdgeFlagSignal(bool bEnable);
};
#endif // WIDGET_H
