#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    this->updateIndicators();
    this->ui->pushButton_2->setStyleSheet(QString("QPushButton { background-color : ") + (this->scrollHorizontal ? "yellow; color : black; }" : "red; color : white; }"));
    this->ui->pushButton_3->setStyleSheet(QString("QPushButton { background-color : ") + (this->scrollVertical ? "yellow; color : black; }" : "red; color : white; }"));

    newGraphDialog = new AddNewGraphDialog(this);
    newGraphDialog->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::replotFFT()
{
    if (!filePath.isEmpty()) {

        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

        std::ifstream inputStream(filePath.toStdString(), std::ios::binary);

        double index = 0;
        auto getIndex = [&index](double & item) {
            item = index++;
        };

        if (inputStream.is_open()) {

            QVector<float> fftFloat(fileInfo.size() / sizeof (float));
            QVector<double> yAxis(fftFloat.size());
            QVector<double> xAxis(fftFloat.size());

            inputStream.read((char*)fftFloat.data(), fftFloat.size() * sizeof (float));
            std::transform(std::begin(fftFloat), std::end(fftFloat), std::begin(yAxis), [](const float & item) {
                return (double)item;
            });

            std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

            // create graph and assign data to it:
            this->ui->plotter->addGraph();
            this->ui->plotter->graph(0)->setData(xAxis, yAxis);

            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);

            this->ui->plotter->xAxis->setRange(xAxis.front(), xAxis.last());
            this->ui->plotter->yAxis->setRange(*std::min_element(yAxis.begin(), yAxis.end()), \
                                               *std::max_element(yAxis.begin(), yAxis.end()));

            this->ui->plotter->replot();

            return true;
        }
    }
    return false;
}

bool MainWindow::replotRAW()
{
    if (!filePath.isEmpty()) {

        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

#ifdef DOUBLE
        typedef struct {
            double I;
            double Q;
        } CPLX;

        QVector<CPLX> rawData(fileInfo.size() / sizeof (CPLX));
        QVector<double> indexes(fileInfo.size() / sizeof (CPLX));
#else
        QVector<IQ_t> rawData(fileInfo.size()  / sizeof (IQ_t));
        QVector<double> indexes(fileInfo.size()  / sizeof (IQ_t));
#endif

        QVector<double> yAxis_i(rawData.size()), yAxis_q(rawData.size());

        std::ifstream fft_input(filePath.toStdString(), std::ios::binary);

        if (fft_input.is_open()) {

            fft_input.read((char*)rawData.data(), (fileInfo.size() / 4));

            for (size_t i = 0; i < (size_t)rawData.size(); i++) {
                indexes[i] = i;
                yAxis_i[i] = rawData[i].I;
                yAxis_q[i] = rawData[i].Q;
            }

            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();
            this->ui->plotter->graph(0)->setData(indexes, yAxis_i);
            this->ui->plotter->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
            this->ui->plotter->graph(1)->setData(indexes, yAxis_q);
            this->ui->plotter->graph(1)->setPen(QPen(Qt::red)); // line color blue for first graph

            /// =====================
            // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);
            // make left and bottom axes always transfer their ranges to right and top axes:

            this->ui->plotter->xAxis->setRange(0, (indexes.size() < (32 * 1024)) ? indexes.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(std::min(*std::min_element(yAxis_i.begin(), yAxis_i.end()), *std::min_element(yAxis_q.begin(), yAxis_q.end())), \
                                               std::max(*std::max_element(yAxis_i.begin(), yAxis_i.end()), *std::max_element(yAxis_q.begin(), yAxis_q.end())));

            this->ui->plotter->replot();

            return true;
        }
    }
    return false;
}

bool MainWindow::replotQRAW()
{
    if (!filePath.isEmpty()) {

        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

        typedef struct {
            int16_t IF;
            int16_t QF;
            int16_t IS;
            int16_t QS;
        } quadIQ_t;

        QVector<quadIQ_t> rawData(fileInfo.size() / sizeof (quadIQ_t));
        QVector<double> indexes(fileInfo.size() / sizeof (quadIQ_t));

        QVector<double> yfAxis_i(rawData.size()), yfAxis_q(rawData.size());
        QVector<double> ysAxis_i(rawData.size()), ysAxis_q(rawData.size());

        std::ifstream fft_input(filePath.toStdString(), std::ios::binary);

        if (fft_input.is_open()) {

            fft_input.read((char*)rawData.data(), fileInfo.size());

            size_t counter = 0;
            for (size_t i = 0; i < (size_t)rawData.size(); i += 2) {
                indexes[counter] = counter;
                yfAxis_i[counter] = rawData[i].IF;
                yfAxis_q[counter] = rawData[i].QF;
                ysAxis_i[counter] = rawData[i].IS;
                ysAxis_q[counter] = rawData[i].QS;
                counter++;
                yfAxis_i[counter] = rawData[i+1].IF;
                yfAxis_q[counter] = rawData[i+1].QF;
                ysAxis_i[counter] = rawData[i+1].IS;
                ysAxis_q[counter] = rawData[i+1].QS;
                counter++;
            }

            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();
            this->ui->plotter->graph(0)->setData(indexes, yfAxis_i);
            this->ui->plotter->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
            this->ui->plotter->graph(1)->setData(indexes, yfAxis_q);
            this->ui->plotter->graph(1)->setPen(QPen(Qt::red)); // line color blue for first graph
            this->ui->plotter->graph(2)->setData(indexes, ysAxis_i);
            this->ui->plotter->graph(2)->setPen(QPen(Qt::green)); // line color blue for first graph
            this->ui->plotter->graph(3)->setData(indexes, ysAxis_q);
            this->ui->plotter->graph(3)->setPen(QPen(Qt::cyan)); // line color blue for first graph

            /// =====================
            // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);
            // make left and bottom axes always transfer their ranges to right and top axes:

            this->ui->plotter->xAxis->setRange(0, (indexes.size() < (32 * 1024)) ? indexes.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(std::min(*std::min_element(yfAxis_i.begin(), yfAxis_i.end()), *std::min_element(yfAxis_q.begin(), yfAxis_q.end())), \
                                               std::max(*std::max_element(yfAxis_i.begin(), yfAxis_i.end()), *std::max_element(yfAxis_q.begin(), yfAxis_q.end())));

            this->ui->plotter->replot();

            return true;
        }
    }
    return false;
}

void MainWindow::replotFFTslot(QVector<uint32_t> fft)
{
    if (fftReplotter == false) {
        this->ui->plotter->clearGraphs();
    }

    this->ui->plotter->addGraph();
    QVector<double> yAxis(fft.size());
    QVector<double> xAxis(fft.size());

    std::transform(std::begin(fft), std::end(fft), std::begin(yAxis), [](uint32_t item) {
        return 20.0 * std::log((double)item);
    });

    double index = 0;
    auto getIndex = [&index](double & item) {
        item = index++;
    };
    std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

    this->ui->plotter->graph(0)->setData(xAxis, yAxis);
    this->ui->plotter->graph(0)->setPen(QPen(Qt::blue));

    this->ui->plotter->xAxis2->setVisible(true);
    this->ui->plotter->xAxis2->setTickLabels(false);
    this->ui->plotter->yAxis2->setVisible(true);
    this->ui->plotter->yAxis2->setTickLabels(false);

    if (fftReplotter == false) {
        this->ui->plotter->xAxis->setRange(0, (xAxis.size() < (32 * 1024)) ? xAxis.last() : (double)(32 * 1024));
        this->ui->plotter->yAxis->setRange(*std::min_element(yAxis.begin(), yAxis.end()), \
                                           *std::max_element(yAxis.begin(), yAxis.end()));
        fftReplotter = true;
    }

    this->ui->plotter->replot();
}

bool MainWindow::replotRawFloat() {
    if (!this->filePath.isEmpty()) {
        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

        QVector<float> dataBuffer(fileInfo.size() / sizeof(float));
        QVector<double> yAxis(dataBuffer.size());
        QVector<double> xAxis(dataBuffer.size());

        std::ifstream inputFileStream(filePath.toStdString(), std::ios::binary);

        if (inputFileStream.is_open()) {

            double index = 0;
            auto getIndex = [&index](double & item) {
                item = index++;
            };

            inputFileStream.read((char*)dataBuffer.data(), dataBuffer.size() * sizeof(float));
            std::transform(std::begin(dataBuffer), std::end(dataBuffer), std::begin(yAxis), [] (const float & item) -> double {
                return (double)item;
            });

            std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

            this->ui->plotter->addGraph();
            this->ui->plotter->graph(0)->setData(xAxis, yAxis);
            this->ui->plotter->graph(0)->setPen(QPen(Qt::blue));

            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);

            this->ui->plotter->xAxis->setRange(0, (xAxis.size() < (32 * 1024)) ? xAxis.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(*std::min_element(yAxis.begin(), yAxis.end()), \
                                               *std::max_element(yAxis.begin(), yAxis.end()));

        } else {
            return false;
        }

        return true;
    }
    return false;
}

bool MainWindow::replotRawDouble() {
    if (!this->filePath.isEmpty()) {
        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

        QVector<double> dataBuffer(fileInfo.size() / sizeof(double));
        QVector<double> xAxis(dataBuffer.size());

        std::ifstream inputFileStream(filePath.toStdString(), std::ios::binary);

        if (inputFileStream.is_open()) {

            double index = 0;
            auto getIndex = [&index](double & item) {
                item = index++;
            };

            inputFileStream.read((char*)dataBuffer.data(), dataBuffer.size() * sizeof(double));

            std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

            this->ui->plotter->addGraph();
            this->ui->plotter->graph(0)->setData(xAxis, dataBuffer);
            this->ui->plotter->graph(0)->setPen(QPen(Qt::blue));

            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);

            this->ui->plotter->xAxis->setRange(0, (xAxis.size() < (32 * 1024)) ? xAxis.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(*std::min_element(dataBuffer.begin(), dataBuffer.end()), \
                                               *std::max_element(dataBuffer.begin(), dataBuffer.end()));

        } else {
            return false;
        }

        return true;
    }
    return false;
}

bool MainWindow::replotRawFPlusIQ() {
    if (!this->filePath.isEmpty()) {
        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);
        uint32_t vectorSize;

        std::ifstream inputFileStream(filePath.toStdString(), std::ios::binary);

        if (inputFileStream.is_open()) {

            inputFileStream.read((char*)&vectorSize, sizeof(uint32_t));

            QVector<float> dataFBuffer(vectorSize);
            QVector<IQ_t> dataIQBuffer(vectorSize);

            QVector<double> yFAxis(dataFBuffer.size());
            QVector<double> yIAxis(dataFBuffer.size());
            QVector<double> yQAxis(dataFBuffer.size());
            QVector<double> xAxis(dataFBuffer.size());

            double index = 0;
            auto getIndex = [&index](double & item) {
                item = index++;
            };

            inputFileStream.read((char*)dataFBuffer.data(), dataFBuffer.size() * sizeof(float));
            inputFileStream.read((char*)dataIQBuffer.data(), dataIQBuffer.size() * sizeof(IQ_t));

            std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

            std::transform(std::begin(dataFBuffer), std::end(dataFBuffer), std::begin(yFAxis), [](const float & item) {
                return (double)item;
            });

            QVector<double>::iterator Iitr = std::begin(yIAxis);
            QVector<double>::iterator Qitr = std::begin(yQAxis);
            for (const IQ_t & item : dataIQBuffer) {
                *Iitr = (double)item.I;
                *Qitr = (double)item.Q;

                Iitr++;
                Qitr++;
            }

            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();
            this->ui->plotter->graph(0)->setData(xAxis, yFAxis);
            this->ui->plotter->graph(1)->setData(xAxis, yIAxis);
            this->ui->plotter->graph(2)->setData(xAxis, yQAxis);

            this->ui->plotter->graph(0)->setPen(QPen(Qt::red));
            this->ui->plotter->graph(1)->setPen(QPen(Qt::blue));
            this->ui->plotter->graph(2)->setPen(QPen(Qt::yellow));

            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);

            this->ui->plotter->xAxis->setRange(0, (xAxis.size() < (32 * 1024)) ? xAxis.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(*std::min_element(dataFBuffer.begin(), dataFBuffer.end()), \
                                               *std::max_element(dataFBuffer.begin(), dataFBuffer.end()));

        } else {
            return false;
        }

        return true;
    }
    return false;
}

bool MainWindow::replotRawFloatIQ() {
    if (!this->filePath.isEmpty()) {
        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

        typedef struct {
            float I;
            float Q;
        } fIQ_t;

        QVector<fIQ_t> dataBuffer(fileInfo.size() / sizeof(fIQ_t));
        QVector<double> yIAxis(dataBuffer.size());
        QVector<double> yQAxis(dataBuffer.size());
        QVector<double> xAxis(dataBuffer.size());

        std::ifstream inputFileStream(filePath.toStdString(), std::ios::binary);

        if (inputFileStream.is_open()) {

            double index = 0;
            auto getIndex = [&index](double & item) {
                item = index++;
            };

            inputFileStream.read((char*)dataBuffer.data(), dataBuffer.size() * sizeof(double));

            std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

            QVector<double>::iterator Iitr = std::begin(yIAxis);
            QVector<double>::iterator Qitr = std::begin(yQAxis);
            for (const fIQ_t & item : dataBuffer) {
                *Iitr = (double)item.I;
                *Qitr = (double)item.Q;

                Iitr++;
                Qitr++;
            }

            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();
            this->ui->plotter->graph(0)->setData(xAxis, yIAxis);
            this->ui->plotter->graph(1)->setData(xAxis, yQAxis);

            this->ui->plotter->graph(0)->setPen(QPen(Qt::blue));
            this->ui->plotter->graph(1)->setPen(QPen(Qt::red));

            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);

            this->ui->plotter->xAxis->setRange(0, (xAxis.size() < (32 * 1024)) ? xAxis.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(*std::min_element(yIAxis.begin(), yIAxis.end()), \
                                               *std::max_element(yIAxis.begin(), yIAxis.end()));

        } else {
            return false;
        }

        return true;
    }
    return false;
}

bool MainWindow::replotSourceDirIQ() {
    if (!this->filePath.isEmpty()) {
        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

        typedef struct {
            float I;
            float Q;
        } fIQ_t;

        std::ifstream inputFileStream(filePath.toStdString(), std::ios::binary);

        if (inputFileStream.is_open()) {

            uint32_t vecSize = 0;
            inputFileStream.read((char*)&vecSize, sizeof (uint32_t));

            QVector<float> sourceSignal(vecSize);
            QVector<float> diffSignal(vecSize);
            QVector<double> xAxis(vecSize);

            QVector<double> yIIAxis(vecSize);
            QVector<double> yFIAxis(vecSize);

            double index = 0;
            auto getIndex = [&index](double & item) {
                item = index++;
            };

            inputFileStream.read((char*)sourceSignal.data(), sourceSignal.size() * sizeof(float));
            inputFileStream.read((char*)diffSignal.data(), diffSignal.size() * sizeof(float));

            std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

            QVector<double>::iterator Iitr = std::begin(yIIAxis);
            for (const float & item : sourceSignal) {
                *Iitr = (double)item;

                Iitr++;
            }

            QVector<double>::iterator fIitr = std::begin(yFIAxis);
            for (const float & item : diffSignal) {
                *fIitr = (double)item;

                fIitr++;
            }

            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();

            this->ui->plotter->graph(0)->setData(xAxis, yIIAxis);
            this->ui->plotter->graph(1)->setData(xAxis, yFIAxis);

            this->ui->plotter->graph(0)->setPen(QPen(Qt::blue));
            this->ui->plotter->graph(1)->setPen(QPen(Qt::red));

            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);

            this->ui->plotter->xAxis->setRange(0, (xAxis.size() < (32 * 1024)) ? xAxis.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(*std::min_element(yFIAxis.begin(), yFIAxis.end()), \
                                               *std::max_element(yFIAxis.begin(), yFIAxis.end()));

        } else {
            return false;
        }

        return true;
    }
    return false;
}

bool MainWindow::replotUInt32()
{
    if (!this->filePath.isEmpty()) {
        this->ui->plotter->clearGraphs();

        QFileInfo fileInfo(filePath);

        std::ifstream inputFileStream(filePath.toStdString(), std::ios::binary);

        if (inputFileStream.is_open()) {

            uint32_t vecSize = fileInfo.size() / sizeof (uint32_t);

            QVector<uint32_t> sourceSignal(vecSize);
            QVector<double> xAxis(vecSize);
            QVector<double> yAxis(vecSize);

            double index = 0;
            auto getIndex = [&index](double & item) {
                item = index++;
            };

            inputFileStream.read((char*)sourceSignal.data(), sourceSignal.size() * sizeof (uint32_t));

            std::for_each(std::begin(xAxis), std::end(xAxis), getIndex);

            QVector<double>::iterator Iitr = std::begin(yAxis);
            for (const uint32_t & item : sourceSignal) {
                *Iitr = (double)item;
                Iitr++;
            }

            this->ui->plotter->addGraph();
            this->ui->plotter->addGraph();

            this->ui->plotter->graph(0)->setData(xAxis, yAxis);

            this->ui->plotter->graph(0)->setPen(QPen(Qt::blue));

            this->ui->plotter->xAxis2->setVisible(true);
            this->ui->plotter->xAxis2->setTickLabels(false);
            this->ui->plotter->yAxis2->setVisible(true);
            this->ui->plotter->yAxis2->setTickLabels(false);

            this->ui->plotter->xAxis->setRange(0, (xAxis.size() < (32 * 1024)) ? xAxis.last() : (double)(32 * 1024));
            this->ui->plotter->yAxis->setRange(*std::min_element(yAxis.begin(), yAxis.end()), \
                                               *std::max_element(yAxis.begin(), yAxis.end()));

        } else {
            return false;
        }

        return true;
    }
    return false;
}

void MainWindow::on_actionFFT_result_triggered()
{
    lastFunction = FFT_float;
    filePath = QFileDialog::getOpenFileName(this, tr("Выберите FFT спектрограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotFFT())
        this->addIfNotExist(filePath, lastFunction);
}


void MainWindow::on_actionRaw_data_triggered()
{
    lastFunction = RAW_iIQ_t;
    filePath = QFileDialog::getOpenFileName(this, tr("Выберите i_IQ осциллограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotRAW())
        this->addIfNotExist(filePath, lastFunction);
}

void MainWindow::on_actionRaw_quad_triggered()
{
    lastFunction = RAW_qiIQ_t;

    filePath = QFileDialog::getOpenFileName(this, tr("Выберите Qi_IQ осцилограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotQRAW())
        this->addIfNotExist(filePath, lastFunction);
}

void MainWindow::on_actionFloat_discretes_triggered()
{
    lastFunction = RAW_float;

    filePath = QFileDialog::getOpenFileName(this, tr("Выберите float осциллограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotRawFloat())
        this->addIfNotExist(filePath, lastFunction);
}


void MainWindow::on_actionDouble_discretes_triggered()
{
    lastFunction = RAW_double;

    filePath = QFileDialog::getOpenFileName(this, tr("Выберите double осциллограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotRawDouble())
        this->addIfNotExist(filePath, lastFunction);
}


void MainWindow::on_actionFloat_raw_IQ_triggered()
{
    lastFunction = RAW_float_iIQ;

    filePath = QFileDialog::getOpenFileName(this, tr("Выберите double осциллограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.frw_iq)"));

    if (replotRawFPlusIQ())
        this->addIfNotExist(filePath, lastFunction);
}

void MainWindow::on_actionFloat_IQ_triggered()
{
    lastFunction = RAW_fIQ_t;

    filePath = QFileDialog::getOpenFileName(this, tr("Выберите float_IQ осциллограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotRawFloatIQ())
        this->addIfNotExist(filePath, lastFunction);
}

void MainWindow::on_actionSource_Diff_fIQ_t_triggered()
{
    lastFunction = SOURCE_DIFF_t;

    filePath = QFileDialog::getOpenFileName(this, tr("Выберите float_IQ осциллограмму для отображения"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotSourceDirIQ())
        this->addIfNotExist(filePath, lastFunction);
}

void MainWindow::on_actionPrint_uint_triggered()
{
    lastFunction = RAW_uint32_t;
    filePath = QFileDialog::getOpenFileName(this, tr("Выберите осциллограмму"), \
                                            QApplication::applicationDirPath(), \
                                            tr("Binary files (*.*)"));

    if (replotUInt32())
        this->addIfNotExist(filePath, lastFunction);
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Control:
        sizingHorizontal = !sizingHorizontal;
        break;
    case Qt::Key_Shift:
        sizingVertical = !sizingVertical;
        break;
    default:
        break;
    }

    this->updateIndicators();

    this->ui->plotter->axisRect()->setRangeZoom( \
                (sizingVertical ? Qt::Vertical : Qt::Orientations(0)) | \
                (sizingHorizontal ? Qt::Horizontal : Qt::Orientations(0)));

}

void MainWindow::on_pushButton_clicked()
{
    // Update button
    switch (lastFunction) {
    case 0:
        replotFFT();
        break;
    case 1:
        replotRAW();
        break;
    case 2:
        replotQRAW();
        break;
    default:
        break;
    }
}

void MainWindow::on_actionFullscreen_triggered()
{
    if (this->isFullScreen()) {
        this->showNormal();
    } else {
        this->showFullScreen();
    }
}

void MainWindow::on_actionClose_window_triggered()
{
    this->close();
}

void MainWindow::updateIndicators()
{
    this->ui->verticalIndicator->setStyleSheet(QString("QLabel { background-color : ") + (sizingVertical ? "yellow" : "red") + "; }");
    this->ui->horizontalIndicator->setStyleSheet(QString("QLabel { background-color : ") + (sizingHorizontal ? "yellow" : "red") + "; }");
}

void MainWindow::addIfNotExist(QString path, int mode)
{
    if (this->ui->listWidget->findItems(path, Qt::MatchFixedString).empty()) {
        switch (mode) {
        case FFT_float:
            this->ui->listWidget->addItem("[FFT]" + path);
            break;
        case RAW_iIQ_t:
            this->ui->listWidget->addItem("[RAW]" + path);
            break;
        case RAW_qiIQ_t:
            this->ui->listWidget->addItem("[QRAW]" + path);
            break;
        case RAW_float:
            this->ui->listWidget->addItem("[FLOAT]" + path);
            break;
        case RAW_double:
            this->ui->listWidget->addItem("[DOUBLE]" + path);
            break;
        case RAW_float_iIQ:
            this->ui->listWidget->addItem("[FLOAT+IQ]" + path);
            break;
        case RAW_fIQ_t:
            this->ui->listWidget->addItem("[FRAW]" + path);
            break;
        case SOURCE_DIFF_t:
            this->ui->listWidget->addItem("[SOURCE_DIFF_t]" + path);
            break;
        case RAW_uint32_t:
            this->ui->listWidget->addItem("[SOURCE_DIFF_t]" + path);
            break;
        default:
            break;
        }

        fileHistory.append(fileHist_t{path, mode});
    }
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    (void)item;
    this->filePath = this->fileHistory.at(this->ui->listWidget->currentRow()).path;
    this->lastFunction = this->fileHistory.at(this->ui->listWidget->currentRow()).type;
}


void MainWindow::on_pushButton_2_clicked()
{
    this->scrollHorizontal = !this->scrollHorizontal;
    this->ui->pushButton_2->setStyleSheet(\
                QString("QPushButton { background-color : ") + (this->scrollHorizontal ? "yellow; color : black; }" : "red; color : white; }"));
    this->ui->plotter->axisRect()->setRangeDrag( \
                (scrollVertical ? Qt::Vertical : Qt::Orientations(0)) | \
                (scrollHorizontal ? Qt::Horizontal : Qt::Orientations(0)));
}


void MainWindow::on_pushButton_3_clicked()
{
    this->scrollVertical = !this->scrollVertical;
    this->ui->pushButton_3->setStyleSheet(\
                QString("QPushButton { background-color : ") + (this->scrollVertical ? "yellow; color : black; }" : "red; color : white; }"));
    this->ui->plotter->axisRect()->setRangeDrag( \
                (scrollVertical ? Qt::Vertical : Qt::Orientations(0)) | \
                (scrollHorizontal ? Qt::Horizontal : Qt::Orientations(0)));
}
