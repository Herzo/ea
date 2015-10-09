#include "freedialog.h"
#include "ui_freedialog.h"

CFreeDialog::CFreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CFreeDialog)
{
    ui->setupUi(this);
}

CFreeDialog::~CFreeDialog()
{
    delete ui;
}
