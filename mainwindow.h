#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDevice>
#include <QFileDialog>
#include <QString>
#include <QFileInfo>
#include <QPushButton>
#include <QListWidget>
#include <QList>
#include <QMetaType>

#include <fstream>
#include <iostream>
#include <thread>
#include <atomic>

#include "addnewgraphdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @typedef IQ_t
 * @brief Структура данных записей I/Q
 */
typedef struct {
    int16_t I;
    int16_t Q;
} IQ_t;

/**
 * @typedef fileHist_t
 * @brief Структура для сохранения истории открываемых файлов
 */
typedef struct {
    QString path;
    int type;
} fileHist_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    AddNewGraphDialog * newGraphDialog;

    bool fftReplotter = false;

    enum MainWindow_PlotModes_ : int {
        FFT_float,
        RAW_iIQ_t,
        RAW_fIQ_t,
        RAW_qiIQ_t,
        RAW_float,
        RAW_double,
        RAW_float_iIQ,
        SOURCE_DIFF_t,
        RAW_uint32_t,
    };

    bool sizingHorizontal{true};
    bool sizingVertical{true};

    bool scrollHorizontal{true};
    bool scrollVertical{true};

    QString filePath;
    int lastFunction{0};
    //    ConsoleWindow * console;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool replotFFT(void);

    bool replotRAW(void);

signals:
    void printToConsole(QString message);

protected:
    bool replotRawFloat();
    bool replotRawDouble();
    bool replotRawFPlusIQ();
    bool replotRawFloatIQ();
    bool replotSourceDirIQ();
    bool replotUInt32();
protected slots:
    bool replotQRAW();
    void replotFFTslot(QVector<uint32_t> fft);
private slots:
    void on_actionFFT_result_triggered();

    void on_actionRaw_data_triggered();

    void keyPressEvent(QKeyEvent *event);

    void on_pushButton_clicked();

    void on_actionFullscreen_triggered();

    void on_actionClose_window_triggered();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_actionRaw_quad_triggered();

    void on_actionFloat_discretes_triggered();

    void on_actionDouble_discretes_triggered();

    void on_actionFloat_raw_IQ_triggered();

    void on_actionFloat_IQ_triggered();

    void on_actionSource_Diff_fIQ_t_triggered();

    void on_actionPrint_uint_triggered();

private:
    void updateIndicators(void);

    void addIfNotExist(QString path, int mode);

    Ui::MainWindow *ui;
    QList<fileHist_t> fileHistory;
};
#endif // MAINWINDOW_H
