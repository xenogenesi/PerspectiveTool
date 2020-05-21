#include "proppoint.h"

#include "ui_proppoint.h"

//#include <QtUiTools/QUiLoader>

PropPoint::PropPoint(int index, PerspectiveEditor *editor) :
    CollapsiblePanel(0),
    ui(new Ui::PropPoint),
    m_index(index)
{
    m_editor = editor;
    m_collapse->setText(QString("Point %1").arg(QString::number(index + 1)));

    if (m_widget == nullptr) {
        m_widget = new QWidget;
    }
    ui->setupUi(m_widget);
    m_widget->setMinimumSize(m_widget->width(),m_widget->height());
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_vlayout->addWidget(m_widget);

    //connect(editor, &PerspectiveEditor::pointMoved, this, &PropPoint::moved);
    connect(editor, SIGNAL(pointMoved(int, const QPointF &)), this, SLOT(moved(int, const QPointF &)));

    connect(ui->posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PropPoint::changed);
    connect(ui->posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PropPoint::changed);

    //connect(this, SIGNAL(move(int, const QPointF &)), editor, SLOT(pointMove(int, const QPointF &)));
    connect(this, SIGNAL(move(const QPointF &)), editor->perspective_points[m_index], SLOT(move(const QPointF &)));
}

PropPoint::~PropPoint()
{
    delete ui;
}

//void PropPoint::setIndex(int index)
//{
//    m_index = index;
//}

//void PropPoint::setEditor(PerspectiveEditor *editor)
//{

//}

void PropPoint::moved(int index, const QPointF &to)
{
    if (index != m_index)
        return;

//    qDebug() << "moved: " << index << " - " << to;

    double x = to.x(), y = to.y();
    if (x != ui->posX->value())
        ui->posX->setValue(x);
    if (y != ui->posY->value())
        ui->posY->setValue(y);
}

void PropPoint::changed(double ignored)
{
    Q_UNUSED(ignored);
    QPointF pos;

    pos.setX(ui->posX->value());
    pos.setY(ui->posY->value());

//    qDebug() << "value changed: " << m_index << " - " << pos;
//    emit move(m_index, pos);
    emit move(pos);
}
