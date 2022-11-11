#include "addnewgraphdialog.h"
#include "ui_addnewgraphdialog.h"

AddNewGraphDialog::AddNewGraphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewGraphDialog)
{
    ui->setupUi(this);

    this->ui->dataType->addItems(QStringList{"Float discretes", "Double discretes"});
}

AddNewGraphDialog::~AddNewGraphDialog()
{
    delete ui;
}

void AddNewGraphDialog::on_graphType_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        this->ui->dataType->clear();
        this->ui->dataType->addItems(QStringList{"Float discretes", "Double discretes"});
        break;
    case 1:
        this->ui->dataType->clear();
        this->ui->dataType->addItems(QStringList{"int16_t IQ", "int32_t IQ", "float IQ", "double IQ"});
        break;
    default:
        break;
    }
}

