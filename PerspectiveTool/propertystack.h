#ifndef PROPERTYSTACK_H
#define PROPERTYSTACK_H

#include <QObject>
#include <QScrollArea>
#include <QWidget>
#include <QList>
#include "collapsiblepanel.h"

class PropertyStack : public QScrollArea
{
    Q_OBJECT
public:
    explicit PropertyStack(QScrollArea *parent = nullptr);
    ~PropertyStack();
    void appendPanel(CollapsiblePanel *panel);

signals:

public slots:
    void doLayout();

private:
    QList<CollapsiblePanel *> m_children;
    QWidget *m_container;
};

#endif // PROPERTYSTACK_H
