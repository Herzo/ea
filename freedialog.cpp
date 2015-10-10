#include "freedialog.h"
#include "ui_freedialog.h"

CFreeDialog::CFreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CFreeDialog)
{
    ui->setupUi(this);
    createActions();
    createTrayIcon();
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    m_pTrayIcon->show();
    InitIconStore();

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

CFreeDialog::InitIconStore()
{

    m_IconStore.insert(QIcon(":/images/idle.svg"),pair("Idle",""));
    m_IconStore.insert(QIcon(":/images/running.svg"),pair("Running",""));
    m_IconStore.insert(QIcon(":/images/warning.svg"),pair("Warning",""));
    m_IconStore.insert(QIcon(":/images/bypass.svg"),pair("Bypass",""));
    m_IconStore.insert(QIcon(":/images/blocking.svg"),pair("Blocking",""));


}
void COptionsDialog::setVisible(bool visible)
{
    m_pMinimizeAction->setEnabled(visible);
    m_pMaximizeAction->setEnabled(!isMaximized());
    m_pRestoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void COptionsDialog::closeEvent(QCloseEvent *event)
{
    if (m_pTrayIcon->isVisible()) {
        QMessageBox::information(this, tr("Einstein's Agent"),
                                 tr("Einstein's Agent will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}

void COptionsDialog::setIcon(int index)
{
    QIcon icon = m_pIconComboBox->itemIcon(index);
    m_pTrayIcon->setIcon(icon);
    setWindowIcon(icon);

    m_pTrayIcon->setToolTip(m_pIconComboBox->itemText(index));
}

void COptionsDialog::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        m_pIconComboBox->setCurrentIndex((m_pIconComboBox->currentIndex() + 1)
                                      % m_pIconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}
