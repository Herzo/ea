#include "multiplicationdialog.h"
#include "ui_multiplicationdialog.h"

CMultiplicationDialog::CMultiplicationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CMultiplicationDialog)
{
    ui->setupUi(this);
}

CMultiplicationDialog::~CMultiplicationDialog()
{
    delete ui;
}
