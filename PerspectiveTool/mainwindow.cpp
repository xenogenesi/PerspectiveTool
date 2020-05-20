#include "mainwindow.h"
#include "perspectiveeditor.h"

#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QtUiTools/QUiLoader>
#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFileDialog>
#include <QSvgGenerator>

#include "collapsiblepanel.h"
#include "propertystack.h"
#include "proppoint.h"
#include "propcanvas.h"

void MainWindow::createDockWindows() {
    QDockWidget *dock = new QDockWidget(tr("Properties"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//    dock->setContentsMargins(0, 0, 0, 0);

    PropertyStack *area = new PropertyStack;
//    area->setContentsMargins(0, 0, 0, 0);

    dock->setWidget(area);
    //dock->setMaximumWidth(area->maximumWidth());

    addDockWidget(Qt::RightDockWidgetArea, dock);

#if 1
    {
        PropCanvas *widget = new PropCanvas(ui->perspectiveEditor);
        area->appendPanel(static_cast<CollapsiblePanel *>(widget));
    }

    {
        PropPoint *widget3 = new PropPoint(0, ui->perspectiveEditor);
        area->appendPanel(static_cast<CollapsiblePanel *>(widget3));
    }

    {
        PropPoint *widget3 = new PropPoint(1, ui->perspectiveEditor);
        area->appendPanel(static_cast<CollapsiblePanel *>(widget3));
    }

    {
        PropPoint *widget3 = new PropPoint(2, ui->perspectiveEditor);
        area->appendPanel(static_cast<CollapsiblePanel *>(widget3));
    }

    area->doLayout();
#endif
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    createDockWindows();

    connect(ui->perspectiveSelectGroup, SIGNAL(buttonClicked(int)), ui->perspectiveEditor, SLOT(perspectivePointsSelected(int)));
    connect(ui->antialiasingCheckBox, SIGNAL(clicked(bool)), ui->perspectiveEditor, SLOT(setAntialiasing(bool)));

    connect(ui->perspectiveEditor, SIGNAL(setLines(int)), ui->lineAmountSlider, SLOT(setValue(int)));
    connect(ui->perspectiveEditor, SIGNAL(setOpacity(int)), ui->opacitySlider, SLOT(setValue(int)));
    connect(ui->perspectiveEditor, SIGNAL(setCenterRemoval(int)), ui->centerRemovalSlider, SLOT(setValue(int)));

    connect(ui->perspectiveEditor, SIGNAL(perspectivePointActivationState(bool)), ui->lineAmountSlider, SLOT(setEnabled(bool)));
    connect(ui->perspectiveEditor, SIGNAL(perspectivePointActivationState(bool)), ui->opacitySlider, SLOT(setEnabled(bool)));
    connect(ui->perspectiveEditor, SIGNAL(perspectivePointActivationState(bool)), ui->centerRemovalSlider, SLOT(setEnabled(bool)));

    connect(ui->lineAmountSlider, SIGNAL(valueChanged(int)), ui->perspectiveEditor, SLOT(setCurrentLines(int)));
    connect(ui->opacitySlider, SIGNAL(valueChanged(int)), ui->perspectiveEditor, SLOT(setCurrentOpacity(int)));
    connect(ui->centerRemovalSlider, SIGNAL(valueChanged(int)), ui->perspectiveEditor, SLOT(setCurrentCenterRemoval(int)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionCopy_to_clipboard_triggered() {
   // ui->perspectiveEditor->renderImage();

    QRectF canvas(ui->perspectiveEditor->m_canvas);
    QImage image = ui->perspectiveEditor->renderImage(canvas, canvas.width(), canvas.height());
    QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
}

void MainWindow::on_actionCopy_to_clipboard_with_size_triggered() {
    SizeDialog dialog;

    dialog.exec();

    QImage image = ui->perspectiveEditor->renderImage(ui->perspectiveEditor->m_canvas, dialog.getCanvasSize().width(), dialog.getCanvasSize().height());
    QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
}

void MainWindow::on_actionExportSvg_triggered() {
    QString path;           // The path to save the file

    QString newPath = QFileDialog::getSaveFileName(this, QString::fromUtf8("Save SVG"),
        path, tr("SVG files (*.svg)"));

    if (newPath.isEmpty())
        return;

    path = newPath;

//    QSize size(ui->perspectiveEditor->m_canvas.width(), ui->perspectiveEditor->m_canvas.height());
    QRectF canvas(ui->perspectiveEditor->m_canvas);
    QSvgGenerator generator;        // Create a file generator object
    generator.setFileName(path);    // We set the path to the file where to save vector graphics
    generator.setSize(QSize(canvas.width(), canvas.height()));  // Set the dimensions of the working area of the document in millimeters
    generator.setViewBox(QRect(0, 0, canvas.width(), canvas.height())); // Set the work area in the coordinates
    generator.setTitle(QString::fromUtf8("SVG Example"));                          // The title document
    generator.setDescription(QString::fromUtf8("File created by SVG Example"));

    QPainter painter;
    painter.begin(&generator);
    //scene->render(&painter);
    ui->perspectiveEditor->paintImage(painter, canvas, canvas.width(), canvas.height(), false);
    painter.end();
}
