#include "controlswindow.h"
#include "ui_controlswindow.h"

ControlsWindow::ControlsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ControlsWindow)
{
    ui->setupUi(this);
}

ControlsWindow::~ControlsWindow()
{
    delete ui;
}
