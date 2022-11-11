#ifndef CONTROLSWINDOW_H
#define CONTROLSWINDOW_H

#include <QMainWindow>

namespace Ui {
class ControlsWindow;
}

class ControlsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ControlsWindow(QWidget *parent = nullptr);
    ~ControlsWindow();

private:
    Ui::ControlsWindow *ui;
};

#endif // CONTROLSWINDOW_H
