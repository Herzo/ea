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

void CFreeDialog::on_pushButton_clicked()
{
    QPixmap imgTick(":/images/tick.png");
    ui->imgMark->setPixmap(imgTick);
}

void CFreeDialog::on_buttonBox_accepted()
{

}

void CFreeDialog::on_CloseButton_clicked()
{

}
