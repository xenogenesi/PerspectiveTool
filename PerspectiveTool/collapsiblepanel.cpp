#include "collapsiblepanel.h"

#include <QFile>
#include <QUiLoader>

CollapsiblePanel::CollapsiblePanel(QWidget *parent)
    : QWidget(parent), m_collapse(nullptr), m_widget(nullptr)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    this->setContentsMargins(0, 0, 0, 0);
    QPushButton *collapse = new QPushButton("Collapse", this);

    collapse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_collapse = collapse;

//    QWidget *widget = new QWidget;
//    m_widget = widget;
//    widget->setMinimumSize(300, 200);

    connect(collapse, SIGNAL(clicked()), this, SLOT(toggle()));

    collapse->setStyleSheet("background-color: chartreuse;");
//    widget->setStyleSheet("background-color: red;");
    this->setStyleSheet("background-color: cyan;");

    QVBoxLayout *vlayout = new QVBoxLayout;
    m_vlayout = vlayout;
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setSizeConstraint(QLayout::SetMinimumSize);
    vlayout->setSpacing(0);
    this->setLayout(vlayout);

    vlayout->addWidget(collapse);
//    vlayout->addWidget(widget);
}

void CollapsiblePanel::loadUi(const QString &name)
{
    QUiLoader loader;
    QFile file(name);
    file.open(QFile::ReadOnly);
    m_widget = loader.load(&file);
//    QWidget *widget1 = loader.load(&file);
    m_widget->setMinimumSize(m_widget->width(),m_widget->height());
//    qDebug() << "widget1 WxH: " << widget1->width() << " x " << widget1->height();
    file.close();
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_vlayout->addWidget(m_widget);
}

void CollapsiblePanel::setEditor(PerspectiveEditor *editor)
{
    m_editor = editor;
}

void CollapsiblePanel::toggle()
{
    if (m_widget != nullptr) {
        QSize newSize;
        if (m_widget->isHidden()) {
            m_widget->show();
            newSize = QSize(this->width(), m_collapse->height() + m_widget->height());
        } else {
            m_widget->hide();
            newSize = QSize(this->width(), m_collapse->height());
        }
        m_collapse->setMinimumWidth(newSize.width());
        this->setMinimumSize(newSize);
        this->resize(newSize);
        emit toggled();
    }
}
