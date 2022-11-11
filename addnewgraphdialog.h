#ifndef ADDNEWGRAPHDIALOG_H
#define ADDNEWGRAPHDIALOG_H

#include <QDialog>

namespace Ui {
class AddNewGraphDialog;
}

class AddNewGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewGraphDialog(QWidget *parent = nullptr);
    ~AddNewGraphDialog();

private slots:
    void on_graphType_currentIndexChanged(int index);
private:
    Ui::AddNewGraphDialog *ui;
};

#endif // ADDNEWGRAPHDIALOG_H
