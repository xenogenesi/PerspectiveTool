#ifndef PROPPOINT_H
#define PROPPOINT_H

#include "collapsiblepanel.h"
#include "perspectiveeditor.h"

namespace Ui {
    class PropPoint;
}

class PropPoint : public CollapsiblePanel
{
    Q_OBJECT
public:
    explicit PropPoint(int index, PerspectiveEditor *editor);
    ~PropPoint();
//    void setIndex(int index);
//    void setEditor(PerspectiveEditor *editor);

signals:
    void move(int index, const QPointF &to);
    void move(const QPointF &to);

public slots:
    void moved(int index, const QPointF &to);
    void changed(double ignored);

private:
    Ui::PropPoint *ui;
    int m_index = -1; // point index in perspectiveeditor vector

};

#endif // PROPPOINT_H
