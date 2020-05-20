#include "perspectiveeditor.h"

PerspectiveEditor::PerspectiveEditor(QWidget *parent) :
    QWidget(parent) {

    perspective_points.resize(3);

    perspective_points[0] = new PerspectivePoint;
    perspective_points[1] = new PerspectivePoint;
    perspective_points[2] = new PerspectivePoint;

    connect(perspective_points[0], SIGNAL(update()), this, SLOT(reRenderCanvasImage()));
    connect(perspective_points[1], SIGNAL(update()), this, SLOT(reRenderCanvasImage()));
    connect(perspective_points[2], SIGNAL(update()), this, SLOT(reRenderCanvasImage()));

    perspective_points[0]->line_color = QColor(200, 50, 50);
    perspective_points[1]->line_color = QColor(50, 200, 50);
    perspective_points[2]->line_color = QColor(50, 50, 200);

    update(); //repaint();

    setFocus();

    reRenderCanvasImage();
}

PerspectiveEditor::~PerspectiveEditor() {
    for (auto point : perspective_points)
      delete point;
    perspective_points.clear();
}

void PerspectiveEditor::selectPerspectivePoint(int p) {
    perspective_point_move = p;
    perspective_point_activated = p;

    emit setOpacity(perspective_points[perspective_point_activated]->opacity);
    emit setLines(perspective_points[perspective_point_activated]->number_of_lines);
    emit setCenterRemoval(perspective_points[perspective_point_activated]->center_removal);

    emit perspectivePointActivationState(true);
}


void PerspectiveEditor::perspectivePointsSelected (int perspective) {
    if(perspective == -2) {
        perspective_point_count = 1;
    } else if(perspective == -3) {
        perspective_point_count = 2;
    } else if(perspective == -4) {
        perspective_point_count = 3;
    }

    reRenderCanvasImage();
}

void PerspectiveEditor::setAntialiasing(bool antialiasing) {
    this->antialiasing = antialiasing;
    reRenderCanvasImage();
}

void PerspectiveEditor::setCurrentOpacity(int opacity) {
    perspective_points[perspective_point_activated]->opacity = opacity;
    reRenderCanvasImage();
}

void PerspectiveEditor::setCurrentLines(int lines) {
    perspective_points[perspective_point_activated]->number_of_lines = lines;
    reRenderCanvasImage();
}

void PerspectiveEditor::setCurrentCenterRemoval(int center_removal) {
    perspective_points[perspective_point_activated]->center_removal = center_removal;
    reRenderCanvasImage();
}

void PerspectiveEditor::pointMove(int index, const QPointF &to)
{
    qDebug() << "pointMove: " << index << " - " << to;
    perspective_points[index]->position = to;
    reRenderCanvasImage();
}

void PerspectiveEditor::resizeCanvas(const QSizeF &size)
{
    m_canvas.setSize(size);
    reRenderCanvasImage();
}

void PerspectiveEditor::wheelEvent (QWheelEvent *event) {
//    QPointF mouse_before = event->pos();
//    QPointF mouse_after;

    if (event->delta() < 0) {
        decreaseZoomLevel();
    } else {
        raiseZoomLevel();
    }
    reRenderCanvasImage();
}

void PerspectiveEditor::raiseZoomLevel () {
    zoom_level += zoom_level_change;

//    reRenderCanvasImage();
}

void PerspectiveEditor::decreaseZoomLevel () {
    zoom_level -= zoom_level_change;

    if(zoom_level < zoom_level_change)
        zoom_level = zoom_level_change;

//    reRenderCanvasImage();
}

QPointF PerspectiveEditor::convertToViewerCoords (QPointF point) {
    QPointF coord_offset = camera_position;

    if(camera_dragging)
        coord_offset -= zoomify(camera_drag_line.p1() - camera_drag_line.p2());

    return zoomify(point) + coord_offset;
}

QPointF PerspectiveEditor::convertToRealCoords (QPointF point) {
    QPointF coord_offset = camera_position;

    if(camera_dragging)
        coord_offset -= dezoomify(camera_drag_line.p1() - camera_drag_line.p2());

    return dezoomify(point - coord_offset);
}

QPointF PerspectiveEditor::zoomify (QPointF point) {
    return point * zoom_level;
}

QPointF PerspectiveEditor::dezoomify (QPointF point) {
    return point / zoom_level;
}

int PerspectiveEditor::zoomify (int value) {
    return value * zoom_level;
}

int PerspectiveEditor::dezoomify (int value) {
    return value / zoom_level;
}

void PerspectiveEditor::startCameraDrag () {
    camera_drag_line.setP1(mouse_view);
    camera_dragging = true;
    setCursor(Qt::ClosedHandCursor);
}

void PerspectiveEditor::endCameraDrag () {
    if(camera_dragging) {
        camera_dragging = false;

        camera_position -= zoomify(camera_drag_line.p1() - camera_drag_line.p2());
    }

    if(camera_drag_mode)
        setCursor(Qt::OpenHandCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void PerspectiveEditor::updateCameraDrag () {
    camera_drag_line.setP2(mouse_view);
}

void PerspectiveEditor::startCameraDragMode () {
    camera_drag_mode = true;
    setCursor(Qt::OpenHandCursor);
}

void PerspectiveEditor::endCameraDragMode () {
    camera_drag_mode = false;
    setCursor(Qt::ArrowCursor);
}

void PerspectiveEditor::mouseMoveEvent(QMouseEvent *event) {
    mouse = convertToRealCoords(event->pos());
    mouse_view = event->pos();

    if(camera_drag_mode && event->buttons() & Qt::MidButton) {
        updateCameraDrag();
        update(); //repaint();
    } else if(perspective_point_move != -1) {
//        perspective_points[perspective_point_move].position = mouse;
        qDebug() << "emit pointMoved: " << perspective_point_move << " - " << mouse;
        pointMove(perspective_point_move, mouse);
        emit pointMoved(perspective_point_move, mouse);
    }
}

void PerspectiveEditor::mousePressEvent(QMouseEvent *event) {
    if (!camera_drag_mode && event->buttons() == Qt::MidButton) {
        startCameraDragMode();
    }
    if(camera_drag_mode) {
        startCameraDrag();
        return;
    }

    for(int p = 0; p < perspective_points.size(); p++) {
        QPointF point = perspective_points[p]->position;

        if(QRectF(convertToViewerCoords(point).x() - 10, convertToViewerCoords(point).y() - 10, 20, 20).contains(mouse_view)) {
            selectPerspectivePoint(p);
            return;
        }
    }
}

void PerspectiveEditor::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    perspective_point_move = -1;

    endCameraDrag();
    if (camera_drag_mode) {
        endCameraDragMode();
    }
    update(); //repaint();
}

void PerspectiveEditor::keyPressEvent(QKeyEvent *event) {
    if(perspective_point_activated != -1) {
        if(event->key() == Qt::Key_Plus) {
            perspective_points[perspective_point_activated]->addLine();
            reRenderCanvasImage();
        }
        if(event->key() == Qt::Key_Minus) {
            perspective_points[perspective_point_activated]->subtractLine();
            reRenderCanvasImage();
        }
    }
}

void PerspectiveEditor::keyReleaseEvent(QKeyEvent *event) {
    Q_UNUSED(event);
}

void PerspectiveEditor::paintImage(QPainter &painter, const QRectF &canvas, int width, int height, bool isScreen) {
    QPointF scaling(width / canvas.width(), height / canvas.height());
    QRectF scaled_canvas (canvas.x() * scaling.x(), canvas.y() * scaling.y(), canvas.width() * scaling.x(), canvas.height() * scaling.y());

    painter.setPen(QPen(Qt::transparent));
    painter.setBrush(QBrush(Qt::white));
    if (isScreen)
        painter.drawRect(scaled_canvas);

    if(antialiasing)
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    for(int p = 0; p < perspective_point_count; p++) {
        QPointF ppoint (perspective_points[p]->position.x() * scaling.x(), perspective_points[p]->position.y() * scaling.y());

        for(int l = 0; l < perspective_points[p]->number_of_lines; l++) {
            QLineF line (ppoint, ppoint + QPointF(100, 100));

            QColor line_color = perspective_points[p]->line_color;

            int base_opacity = perspective_points[p]->opacity;

            if(l % 2) {
                line_color.setAlpha(base_opacity);
            } else {
                line_color.setAlpha(base_opacity - 20);
            }

            painter.setPen(QPen(line_color));

            line.setLength(canvas.width() + canvas.height());
            line.setAngle(360 / (float) perspective_points[p]->number_of_lines * l);

            if(perspective_points[p]->center_removal > 0) {
                QLineF new_line(line.p2(), line.p1());

                new_line.setLength(new_line.length() - perspective_points[p]->center_removal);

                line = new_line;
            }

            {
                QPointF p1(line.p1()), p2(line.p2());
                QLineF new_line, top(0, 0, scaled_canvas.width(), 0),
                        left(0, 0, 0, scaled_canvas.height()),
                        bottom(0, scaled_canvas.height(), scaled_canvas.width(), scaled_canvas.height()),
                        right(scaled_canvas.width(), 0, scaled_canvas.width(), scaled_canvas.height());
                bool found1 = false, found2 = false;

                if ((p1.x() <= 0 && p2.x() <= 0) || (p1.x() >= scaled_canvas.width() && p2.x() >= scaled_canvas.width()))
                    continue;
                if ((p1.y() <= 0 && p2.y() <= 0) || (p1.y() >= scaled_canvas.height() && p2.y() >= scaled_canvas.height()))
                    continue;

                if (scaled_canvas.contains(p1)) {
                    new_line.setP1(p1);
                    found1 = true;
                }
                if (scaled_canvas.contains(p2)) {
                    new_line.setP2(p2);
                    found2 = true;
                }

                if (found1 && found2) {
                    painter.drawLine(new_line);
                } else if (found1) {
                    QPointF intersect;
                    if (line.intersect(top, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP2(intersect);
                        painter.drawLine(new_line);
                    } else if (line.intersect(left, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP2(intersect);
                        painter.drawLine(new_line);
                    } else if (line.intersect(bottom, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP2(intersect);
                        painter.drawLine(new_line);
                    } else if (line.intersect(right, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP2(intersect);
                        painter.drawLine(new_line);
                    }
                } else if (found2) {
                    QPointF intersect;
                    if (line.intersect(top, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP1(intersect);
                        painter.drawLine(new_line);
                    } else if (line.intersect(left, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP1(intersect);
                        painter.drawLine(new_line);
                    } else if (line.intersect(bottom, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP1(intersect);
                        painter.drawLine(new_line);
                    } else if (line.intersect(right, &intersect) == QLineF::BoundedIntersection) {
                        new_line.setP1(intersect);
                        painter.drawLine(new_line);
                    }
                } else {
                    QPointF xTop, xLeft, xBottom, xRight;
                    QLineF::IntersectType tTop, tLeft, tBottom, tRight;

                    tTop = line.intersect(top, &xTop);
                    tLeft = line.intersect(left, &xLeft);
                    tBottom = line.intersect(bottom, &xBottom);
                    tRight = line.intersect(right, &xRight);

                    // remove eventual double intersections at the corners
                    if (tTop == QLineF::BoundedIntersection && tRight == QLineF::BoundedIntersection) {
                        if (xTop == xRight) // invalidate any of two
                            tTop = QLineF::NoIntersection;
                    }
                    if (tRight == QLineF::BoundedIntersection && tBottom == QLineF::BoundedIntersection) {
                        if (xRight == xBottom)
                            tRight = QLineF::NoIntersection;
                    }
                    if (tBottom == QLineF::BoundedIntersection && tLeft == QLineF::BoundedIntersection) {
                        if (xBottom == xLeft)
                            tBottom = QLineF::NoIntersection;
                    }
                    if (tLeft == QLineF::BoundedIntersection && tTop == QLineF::BoundedIntersection) {
                        if (xLeft == xTop)
                            tLeft = QLineF::NoIntersection;
                    }

                    bool foundP1 = false, foundP2 = false;

                    if (foundP1 != true && tTop == QLineF::BoundedIntersection) {
                        new_line.setP1(xTop);
                        tTop = QLineF::NoIntersection; // avoid to reuse it for P2
                        foundP1 = true;
                    } else if (foundP1 != true && tRight == QLineF::BoundedIntersection) {
                        new_line.setP1(xRight);
                        tRight = QLineF::NoIntersection;
                        foundP1 = true;
                    } else if (foundP1 != true && tLeft == QLineF::BoundedIntersection) {
                        new_line.setP1(xLeft);
                        tLeft = QLineF::NoIntersection;
                        foundP1 = true;
                    } else if (foundP1 != true && tBottom == QLineF::BoundedIntersection) {
                        new_line.setP1(xBottom);
                        tBottom = QLineF::NoIntersection;
                        foundP1 = true;
                    }

                    if (foundP2 != true && tTop == QLineF::BoundedIntersection) {
                        new_line.setP2(xTop);
                        foundP2 = true;
                    } else if (foundP2 != true && tRight == QLineF::BoundedIntersection) {
                        new_line.setP2(xRight);
                        foundP2 = true;
                    } else if (foundP2 != true && tLeft == QLineF::BoundedIntersection) {
                        new_line.setP2(xLeft);
                        foundP2 = true;
                    } else if (foundP2 != true && tBottom == QLineF::BoundedIntersection) {
                        new_line.setP2(xBottom);
                        foundP2 = true;
                    }

                    if (foundP1 == true && foundP2 == true)
                        painter.drawLine(new_line);
                }
            }
            //painter.drawLine(line);
        }
    }

    if(perspective_point_count >= 2) {
        QPointF first_point (perspective_points[0]->position.x() * scaling.x(), perspective_points[0]->position.y() * scaling.y());
        QPointF second_point (perspective_points[1]->position.x() * scaling.x(), perspective_points[1]->position.y() * scaling.y());

        if(!scaled_canvas.contains(first_point) && !scaled_canvas.contains(second_point)) {
            painter.setPen(QPen(QColor(30, 30, 180, 100)));
            painter.drawLine(QLineF(first_point, second_point));
        }
    }
}

QImage PerspectiveEditor::renderImage (const QRectF &canvas, int width, int height) {
    QImage image (width, height, QImage::Format_RGB32);

    QPainter canvas_painter (&image);

    paintImage(canvas_painter, canvas, width, height);

    return image;
}

void PerspectiveEditor::reRenderCanvasImage () {
    QRectF canvas(m_canvas);
    canvas_image = renderImage(canvas, canvas.width(), canvas.height()).scaled(zoomify(canvas.width()), zoomify(canvas.height()), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update(); // repaint();
}

void PerspectiveEditor::paintScene(QPainter &painter, const QRectF &canvas) {
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setBrush(QBrush(QColor(120, 120, 120)));

    painter.drawRect(QRect(0, 0, this->width() - 1, this->height() - 1));

    QRectF canvas_rect = QRectF(convertToViewerCoords(QPointF(0, 0)).x(), convertToViewerCoords(QPointF(0, 0)).y(), zoomify(canvas.width()), zoomify(canvas.height()));

    painter.setPen(QPen(Qt::black));
    painter.drawRect(canvas_rect);

    for(int p = 0; p < perspective_point_count; p++) {
        QPointF ppoint = perspective_points[p]->position;

        QPolygonF canvas_polygon(QRectF(0, 0, canvas.width(), canvas.height()));

        if(!canvas_rect.contains(ppoint)) {
            QPointF first_point;
            QPointF second_point;

            if(ppoint.x() > canvas_polygon[2].x() && ppoint.y() > canvas_polygon[2].y()) {
                first_point = canvas_polygon[3];
                second_point = canvas_polygon[1];
            } else if(ppoint.x() > canvas_polygon[3].x() && ppoint.y() > canvas_polygon[3].y()) {
                first_point = canvas_polygon[3];
                second_point = canvas_polygon[2];
            } else if(ppoint.x() < canvas_polygon[0].x() && ppoint.y() < canvas_polygon[0].y()) {
                first_point = canvas_polygon[1];
                second_point = canvas_polygon[3];
            } else if(ppoint.x() < canvas_polygon[3].x() && ppoint.y() > canvas_polygon[3].y()) {
                first_point = canvas_polygon[0];
                second_point = canvas_polygon[2];
            } else if(ppoint.x() < canvas_polygon[0].x() && ppoint.y() > canvas_polygon[0].y()) {
                first_point = canvas_polygon[0];
                second_point = canvas_polygon[3];
            } else if(ppoint.x() > canvas_polygon[1].x() && ppoint.y() < canvas_polygon[1].y()) {
                first_point = canvas_polygon[0];
                second_point = canvas_polygon[2];
            } else if(ppoint.x() > canvas_polygon[0].x() && ppoint.y() < canvas_polygon[0].y()) {
                first_point = canvas_polygon[0];
                second_point = canvas_polygon[1];
            } else if(ppoint.x() > canvas_polygon[1].x() && ppoint.y() > canvas_polygon[1].y()) {
                first_point = canvas_polygon[1];
                second_point = canvas_polygon[2];
            }

            QPen box_line;
            box_line.setWidth(2);
            box_line.setStyle(Qt::DashLine);
            box_line.setColor(QColor(75, 75, 125));

            painter.setPen(box_line);

            painter.drawLine(convertToViewerCoords(ppoint), convertToViewerCoords(first_point));
            painter.drawLine(convertToViewerCoords(ppoint), convertToViewerCoords(second_point));
        }
    }

    if(perspective_point_count >= 2) {
        if(!canvas_rect.contains(perspective_points[0]->position) && !canvas_rect.contains(perspective_points[1]->position)) {

            painter.setPen(QPen(QColor(30, 30, 180)));
            painter.drawLine(QLineF(convertToViewerCoords(perspective_points[0]->position), convertToViewerCoords(perspective_points[1]->position)));
        }
    }

    painter.drawImage(canvas_rect.x(), canvas_rect.y(), canvas_image);
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(QPen(Qt::black));
    painter.drawRect(canvas_rect);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    for(int p = 0; p < perspective_point_count; p++) {
        QPointF ppoint = convertToViewerCoords(perspective_points[p]->position);

        if(perspective_point_activated == p) {
            painter.setBrush(QBrush(QColor(200, 250, 0)));
        } else {
            painter.setBrush(QBrush(QColor(200, 200, 200)));

        }
        painter.setPen(QPen(Qt::black));

        painter.drawEllipse(ppoint, 10, 10);
        painter.drawText(ppoint - QPointF(3, -4), QString::number(p + 1));
    }

    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(QPen(QColor(200, 200, 200)));
    painter.drawRect(QRect(0, 0, this->width() - 1, this->height() - 1));
}

void PerspectiveEditor::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    QRectF canvas(m_canvas);

    paintScene(painter, canvas);
}
