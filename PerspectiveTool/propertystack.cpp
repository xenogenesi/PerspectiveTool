#include "propertystack.h"

#include <QDebug>

#include "proppoint.h"

PropertyStack::PropertyStack(QScrollArea *parent) : QScrollArea(parent), m_container(nullptr)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

    m_container = new QWidget;
    m_container->setStyleSheet("background-color: yellow;");

#if 0
    CollapsiblePanel *widget1, *widget2, *widget3;
    {
        widget1 = new CollapsiblePanel;
        widget1->loadUi(":/forms/test.ui");
        appendPanel(widget1);
    }

    {
        widget2 = new CollapsiblePanel;
        widget2->loadUi(":/forms/test2.ui");
        appendPanel(widget2);
    }

    {
        widget3 = new PropPoint;
        appendPanel(widget3);
    }
#endif

//    container->setMinimumSize(width, height); // FIXME default min size?
    this->setWidget(m_container);

    doLayout();
}

PropertyStack::~PropertyStack()
{
    CollapsiblePanel *item;
    while ((m_children.size() > 0)) {
        item = m_children.takeAt(0);
//        item->setParent(nullptr);
        delete item;
    }
}

void PropertyStack::appendPanel(CollapsiblePanel *panel)
{
    // non aggiungere doppi
    //m_children.indexOf(panel, 0)
    if (m_container != nullptr) {
        panel->setParent(m_container);
    }
    connect(panel, SIGNAL(toggled()), this, SLOT(doLayout()));
    m_children.append(panel);
}

void PropertyStack::doLayout()
{
    CollapsiblePanel *item;
    short int x = 0, y = 0, width = 0, height = 0;
    QSize sz;
    for(int i = 0; i < m_children.size(); i++) {
        item = m_children.at(i);
        sz = item->sizeHint();

        // get the max width (for min container width)
        if (sz.width() > width)
            width = sz.width();

        //item->resize(sz);
        item->setMinimumSize(sz);
        item->move(x, y);
        y += sz.height();
        // accumulate panel height/s (for min container height)
        height += sz.height();
    }
    m_container->setMinimumSize(width, height);
    //this->setMinimumWidth(width);
    this->setMaximumWidth(width + 2);
}
