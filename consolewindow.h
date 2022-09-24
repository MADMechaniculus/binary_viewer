#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <QDialog>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QFileDevice>

#include <libpsm.h>

#include <vector>

namespace Ui {
class ConsoleWindow;
}

class ConsoleWindow : public QDialog
{
    Q_OBJECT

    QString openedFilePath;
    LibPSM psmCalculator;

public:
    explicit ConsoleWindow(QWidget *parent = nullptr);
    ~ConsoleWindow();

public slots:
    void appendConsole(QString message);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    void updateFilePath(void);

    Ui::ConsoleWindow *ui;
};

#endif // CONSOLEWINDOW_H
