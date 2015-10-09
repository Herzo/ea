#include "changeuserdialog.h"
#include "ui_changeuserdialog.h"

CChangeUserDialog::CChangeUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CChangeUserDialog)
{
    ui->setupUi(this);
}

CChangeUserDialog::~CChangeUserDialog()
{
    delete ui;
}
