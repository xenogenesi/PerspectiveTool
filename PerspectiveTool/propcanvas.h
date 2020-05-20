#ifndef PROPCANVAS_H
#define PROPCANVAS_H

#include <QObject>

#include "collapsiblepanel.h"
#include "perspectiveeditor.h"

namespace Ui {
    class PropCanvas;
}

class PropCanvas : public CollapsiblePanel
{
    Q_OBJECT
public:
    PropCanvas(PerspectiveEditor *editor);

signals:
    void resized(const QSizeF &size);

public slots:
    void changed(double ignored);

private:
    Ui::PropCanvas *ui;
};

#endif // PROPCANVAS_H
