#ifndef COLLAPSIBLEPANEL_H
#define COLLAPSIBLEPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "perspectiveeditor.h"

class CollapsiblePanel : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsiblePanel(QWidget *parent = 0);
    void loadUi(const QString &name);
    void setEditor(PerspectiveEditor *editor);

signals:
    void toggled();

public slots:
    void toggle();

protected:
    QPushButton *m_collapse;
    QVBoxLayout *m_vlayout;
    QWidget *m_widget;
    PerspectiveEditor *m_editor;
};

#endif // COLLAPSIBLEPANEL_H
