#include <QSettings>
#include "getwindowtitledialog.h"
#include "ui_getwindowtitledialog.h"

CGetWindowTitleDialog::CGetWindowTitleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGetWindowTitleDialog)
{
    ui->setupUi(this);
    InitSkin();
}

CGetWindowTitleDialog::~CGetWindowTitleDialog()
{
    delete ui;
}

void CGetWindowTitleDialog::on_buttonBox_accepted()
{
    m_sTitle=ui->lineEditTitleText->text();
}
void CGetWindowTitleDialog::InitSkin()
{
    QSettings Settings;
    QPixmap img(Settings.value("skingetwindowtitle",":/images/Dave_106x150.png").toString());
    ui->labelCharacterPicture->setPixmap(img);
}
void CGetWindowTitleDialog::SetHeading(QString sHeading)
{
    ui->labelHeading->setText(sHeading);
}
