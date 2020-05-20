#include "perspectivepoint.h"

PerspectivePoint::PerspectivePoint(QObject *parent) :
    QObject(parent)
{
}

bool PerspectivePoint::inHelperArea(QPointF point) {

    QRectF box(position.x() - 10,  position.y() - 10,  20,  20);

    return box.contains(point);
}

void PerspectivePoint::addLine() {
    number_of_lines += 4;

    if(number_of_lines > 500)
        number_of_lines = 500;
}

void PerspectivePoint::subtractLine() {

    number_of_lines -= 4;

    if(number_of_lines < 0)
        number_of_lines = 0;

}

void PerspectivePoint::move(const QPointF &to)
{
    bool didMove = false;

    if (to.x() != position.x()) {
        position.setX(to.x());
        didMove = true;
    }

    if (to.y() != position.y()) {
        position.setY(to.y());
        didMove = true;
    }

    if (didMove) {
        emit moved(to);
        emit update();
    }
}
