#include "consolewindow.h"
#include "ui_consolewindow.h"

ConsoleWindow::ConsoleWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsoleWindow)
{
//    ui->setupUi(this);

    updateFilePath();

//    this->ui->coef0->setText(QString::number(PXIe::coef1));
//    this->ui->coef_40->setText(QString::number(PXIe::coef2));
//    this->ui->freqLine->setText("1000.0e+6");
}

ConsoleWindow::~ConsoleWindow()
{
    delete ui;
}

void ConsoleWindow::appendConsole(QString message)
{
//    this->ui->consoleText->appendPlainText(message);
}

void ConsoleWindow::on_pushButton_clicked()
{
    QString filePath;

    filePath = QFileDialog::getOpenFileName(this, tr("Выберите файл для обработки"), \
                                            this->openedFilePath.isEmpty() ? \
                                                QApplication::applicationDirPath() : \
                                                QFileInfo(this->openedFilePath).dir().path() \
                                                , tr("Binary files (*.bin *.dat)"));

    if (!filePath.isEmpty())
        this->openedFilePath = filePath;

    updateFilePath();
}

void ConsoleWindow::updateFilePath()
{
    if (this->openedFilePath.isEmpty()) {
        this->ui->selecteFilePath->setText("[EMPTY]");
    } else {
        this->ui->selecteFilePath->setText(this->openedFilePath);
    }
}

void ConsoleWindow::on_pushButton_2_clicked()
{
    double convFreq = this->ui->freqLine->text().isEmpty() ? 0 : this->ui->freqLine->text().toDouble();
    double coef1 = this->ui->coef0->text().toDouble();
    double coef2 = this->ui->coef_40->text().toDouble();

    if (this->openedFilePath.isEmpty())
        this->appendConsole("Сначала, выберите файл!");
    else {
        int errorCode = psmCalculator.loadFile(this->openedFilePath.toStdString());
        if (errorCode == 0) {
            std::ostringstream stringStream;
            std::ostringstream reportStream;
            PULSES_REPORT report;
            psmCalculator.processFile(report, stringStream);

            if (true) {
                reportStream << report;
                this->appendConsole(QString::fromStdString(stringStream.str()));
                this->appendConsole(QString::fromStdString(reportStream.str()));

                double measF = LibPSM::get_f_adc(report.fft_result.size(), report.fft_max_idx, CENT_F, ADC_FS);

                this->appendConsole(QString("Измеренная частота: ") + QString::number(measF) + " Hz");
                this->appendConsole(QString("Частота со смещение: ") + QString::number(((measF < 140.0e+6) ? (convFreq - (140.0e+6 - measF)) : (convFreq + (140.0e+6 - measF)))) + " Hz");

                this->appendConsole(QString("Абсолютное значение в максимальной точке FFT: ") + \
                                    QString::number(*std::max_element(report.fft_result.begin(), report.fft_result.end())));

                std::vector<double> fftBuffer(report.fft_result.size());
                std::copy(report.fft_result.begin(), report.fft_result.end(), fftBuffer.begin());
                LibPSM::translateFFT(fftBuffer, fftBuffer, coef1, coef2);

                this->appendConsole(QString("Максимальное значение амплитуды: " + QString::number(\
                                                *std::max_element(fftBuffer.begin(), fftBuffer.end())) + " dBm"));

                std::vector<double> fft_x_axis(report.fft_result.size());
                for (size_t idx = 0; idx < fft_x_axis.size(); idx++) {
                    fft_x_axis[idx] = LibPSM::get_f_adc(report.fft_result.size(), idx, CENT_F, ADC_FS);
                }

                std::ofstream fftFileOutput(QApplication::applicationDirPath().toStdString() + "\\spectrum.dat", std::ios::binary);

                if (fftFileOutput.is_open()) {
                    fftFileOutput.write((char*)fftBuffer.data(), sizeof (double) * fftBuffer.size());
                    fftFileOutput.write((char*)fft_x_axis.data(), sizeof (double) * fftBuffer.size());
                    fftFileOutput.close();
                    this->appendConsole("Спектрограмма сохранена в:");
                    this->appendConsole(QApplication::applicationDirPath() + "\\spectrum.dat");
                } else {
                    this->appendConsole("Не удалось сохранить спектрограмму в:");
                    this->appendConsole(QApplication::applicationDirPath() + "\\spectrum.dat");
                }

            } else {
                this->appendConsole(QString::fromStdString(stringStream.str()));
            }

        } else {
            std::string message{""};
            try {
                message = "PSM_ERROR: " + errors.at(errorCode);
            } catch (std::exception &e) {
                message = "PSM_ERROR: " + errors.at(0);
            }
            this->appendConsole(QString::fromStdString(message));
        }
    }
}
