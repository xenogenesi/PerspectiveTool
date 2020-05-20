#include "propcanvas.h"

#include "ui_propcanvas.h"

PropCanvas::PropCanvas(PerspectiveEditor *editor) :
    CollapsiblePanel(0),
    ui(new Ui::PropCanvas)
{
    m_editor = editor;
    m_collapse->setText("Canvas");

    if (m_widget == nullptr) {
        m_widget = new QWidget;
    }
    ui->setupUi(m_widget);
    m_widget->setMinimumSize(m_widget->width(),m_widget->height());
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_vlayout->addWidget(m_widget);

//    qDebug() << "canvas size:" << editor->m_canvas;
    ui->canvasWidth->setValue(editor->m_canvas.width());
    ui->canvasHeight->setValue(editor->m_canvas.height());

    connect(ui->canvasWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PropCanvas::changed);
    connect(ui->canvasHeight, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PropCanvas::changed);

    connect(this, SIGNAL(resized(const QSizeF &)), editor, SLOT(resizeCanvas(const QSizeF &)));
}

void PropCanvas::changed(double ignored)
{
    Q_UNUSED(ignored);
    QSizeF newSize(ui->canvasWidth->value(), ui->canvasHeight->value());
    QSizeF oldSize(m_editor->m_canvas.size());

    if (newSize != oldSize) {
        emit resized(newSize);
    }
//    pos.setX(ui->posX->value());
//    pos.setY(ui->posY->value());

//    qDebug() << "value changed: " << m_index << " - " << pos;
//    emit move(m_index, pos);
//    emit move(pos);

}
