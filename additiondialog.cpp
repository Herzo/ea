#include "additiondialog.h"
#include "ui_additiondialog.h"

CAdditionDialog::CAdditionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAdditionDialog)
{
    ui->setupUi(this);
}

CAdditionDialog::~CAdditionDialog()
{
    delete ui;
}
