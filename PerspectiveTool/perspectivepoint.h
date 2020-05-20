#ifndef PERSPECTIVEPOINT_H
#define PERSPECTIVEPOINT_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QLineF>
#include <QColor>
#include <QRectF>
#include <QDebug>

class PerspectivePoint : public QObject
{
    Q_OBJECT

public:
    PerspectivePoint(QObject *parent = nullptr);
    QPointF position;
    QVector<QLineF> lines;
    QColor line_color;

    int number_of_lines = 120;

    int center_removal = 20;

    int opacity = 200;

    void resetLines();
    void createLinesFromTwoPoints(QPointF first_point, QPointF second_point);

    bool inHelperArea (QPointF point);

    void addLine();
    void subtractLine();

signals:
    void moved(const QPointF &to);
    void update();

public slots:
    void move(const QPointF &to);
};

#endif // PERSPECTIVEPOINT_H
