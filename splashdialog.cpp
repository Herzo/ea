#include <QSettings>
#include <QTimer>
#include "splashdialog.h"
#include "ui_splashdialog.h"

CSplashDialog::CSplashDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSplashDialog)
{
    ui->setupUi(this);
    QTimer::singleShot(12000, this, SLOT(on_pushButton_clicked()));
//    move(
//        parentWidget()->window()->frameGeometry().topLeft() +
//        parentWidget()->window()->rect().center() -
//        rect().center());
    QSettings Settings;
    ui->checkBoxShowSplash->setChecked(Settings.value("showsplash",true).toBool());

}

CSplashDialog::~CSplashDialog()
{
    delete ui;
}

void CSplashDialog::on_checkBoxShowSplash_clicked(bool checked)
{
    QSettings Settings;
    Settings.setValue("showsplash",checked);
}

void CSplashDialog::on_pushButton_clicked()
{
    close();
}
