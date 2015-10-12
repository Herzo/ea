#include <QMenu>
#include <QMessageBox>
#include <math.h>
#include <QTimer>
#include<QDebug>

#include "freedialog.h"
#include "ui_freedialog.h"

CFreeDialog::CFreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CFreeDialog)
{
    qsrand(time( NULL ));
    ui->setupUi(this);
    createActions();
    createTrayIcon();
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    m_pTrayIcon->show();
    InitIconStore();
    setIcon("Idle");
    InitQuestion();
    ui->Answer->installEventFilter(this);

}

CFreeDialog::~CFreeDialog()
{
    delete ui;
}


bool CFreeDialog::eventFilter(QObject *watched, QEvent *event)
{
     if (event->type() == QEvent::KeyPress)
     {
         QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

         if (keyEvent->key()==Qt::Key_Return||keyEvent->key()==Qt::Key_Enter)
         {
             QTimer::singleShot(0, this, SLOT(on_NextButton_clicked()));
         }
         else
         {
             return QObject::eventFilter(watched, event);
         }
         return true;
     }
     else
     {
         return QObject::eventFilter(watched, event);
     }
}

void CFreeDialog::on_pushButton_clicked()
{
    QPixmap imgTick(":/images/tick.png");
    ui->imgMark->setPixmap(imgTick);
}

void CFreeDialog::InitIconStore()
{

    m_IconStore.insert("Idle",std::pair< QIcon, QString>(QIcon(":/images/idle.svg"),"No running games"));
    m_IconStore.insert("Running",std::pair< QIcon, QString>(QIcon(":/images/running.svg"),"Using game minutes"));
    m_IconStore.insert("Warning",std::pair< QIcon, QString>(QIcon(":/images/warning.svg"),"Game minutes are running low"));
    m_IconStore.insert("Bypass",std::pair< QIcon, QString>(QIcon(":/images/bypass.svg"),"Not monitoring or regulating game activity"));
    m_IconStore.insert("Blocking",std::pair< QIcon, QString>(QIcon(":/images/blocking.svg"),"Game minutes exhausted"));

}
void CFreeDialog::setVisible(bool visible)
{
    m_pMinimizeAction->setEnabled(visible);
    m_pMaximizeAction->setEnabled(!isMaximized());
    m_pRestoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void CFreeDialog::closeEvent(QCloseEvent *event)
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

void CFreeDialog::setIcon(QString sMode)
{
    QIcon icon = m_IconStore[sMode].first;
    m_pTrayIcon->setIcon(icon);
    setWindowIcon(icon);

    m_pTrayIcon->setToolTip(m_IconStore[sMode].second);
}

void CFreeDialog::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:

        break;
    default:
        ;
    }
}
void CFreeDialog::on_buttonBox_accepted()
{

}

void CFreeDialog::on_CloseButton_clicked()
{
    hide();
}
void CFreeDialog::createActions()
{
    m_pMinimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(m_pMinimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    m_pMaximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(m_pMaximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    m_pRestoreAction = new QAction(tr("&Restore"), this);
    connect(m_pRestoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_pQuitAction = new QAction(tr("&Quit"), this);
    connect(m_pQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}
void CFreeDialog::createTrayIcon()
{
    m_pTrayIconMenu = new QMenu(this);
    m_pTrayIconMenu->addAction(m_pMinimizeAction);
    m_pTrayIconMenu->addAction(m_pMaximizeAction);
    m_pTrayIconMenu->addAction(m_pRestoreAction);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pQuitAction);

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
}

void CFreeDialog::on_NextButton_clicked()
{
    // Check if they have answered correctly
    int iAnswer=ui->Answer->toPlainText().toInt();
    if(iAnswer==m_iAnswer)
    {
        // They answered correctly
        QPixmap imgTick(":/images/tick.png");
        ui->imgMark->setPixmap(imgTick);
        int iGameMinutes = ui->GameMinutesCounter->value();
        ui->GameMinutesCounter->display(iGameMinutes+1);
        InitQuestion();
    }else
    {
        QPixmap imgTick(":/images/cross.png");
        ui->imgMark->setPixmap(imgTick);
        ui->Answer->setPlainText(QString::number(iAnswer));
    }
    QTimer::singleShot(1500, this, SLOT(on_RemoveMark()));
    ui->Answer->setFocus();
}
void CFreeDialog::InitQuestion()
{
    // Generate a new question, and update the dialog
    int iOperand1=(qrand() % 100); //  (qrand() % (max-min)+1) + min;
    int iOperand2=(qrand() % 100);
    int iOperand3=(qrand() % 100);
    int iOperator=(qrand() % 4);
    qDebug() << iOperator;
    int iLocationOfUnknown=(qrand() % 3);
    qDebug() << iLocationOfUnknown;
    QString sOperator="+";
    switch(iOperator)
    {
        case 0:
            sOperator="+";
            iOperand3=iOperand1+iOperand2;
        break;
        case 1:
            sOperator="-";
            iOperand3=iOperand1-iOperand2;
        break;
        case 2:
            sOperator="x";
             iOperand1=(qrand() % 11)+1;
             iOperand2=(qrand() % 11)+1;
            iOperand3=iOperand1*iOperand2;
        break;
        case 3:
            sOperator="/";
            iOperand2=(qrand() % 11)+1;
            iOperand3=(qrand() % 11)+1;
            iOperand1=iOperand2*iOperand3;

    }
    ui->Operand1->setText(QString::number(iOperand1));
    ui->Operand2->setText(QString::number(iOperand2));
    ui->Operand3->setText(QString::number(iOperand3));
    ui->Operator->setText(sOperator);
    switch(iLocationOfUnknown)
    {
        case 0:
        // ? + 4 = 10
            ui->Operand1->setText("?");
            m_iAnswer=iOperand1;
//            iOperator=InverseOperator(iOperator);
//            m_iAnswer=Operation(iOperand3,iOperator,iOperand2);
        break;
        case 1:
        // 6 + ? = 10
            ui->Operand2->setText("?");
            m_iAnswer=iOperand2;
//            iOperator=InverseOperator(iOperator);
//            m_iAnswer=Operation(iOperand3,iOperator,iOperand1);
        break;
        case 2:
        // 6 + 4 = ?
            ui->Operand3->setText("?");
            m_iAnswer=iOperand3;
//            m_iAnswer=Operation(iOperand1,iOperator,iOperand2);
    }
    ui->Answer->setPlainText("");
}
int CFreeDialog::InverseOperator(int iOperator)
{
    switch(iOperator)
    {
        case 0:
        // +
            return 1;
        case 1:
        // -
            return 0;
        case 2:
        // x
            return 3;
        case 3:
        // /
            return 2;
    }
return 0;
}
int CFreeDialog::Operation(int iOperand1,int iOperator,int iOperand2)
{
    switch(iOperator)
    {
        case 0:
        // +
            return (iOperand1+iOperand2);
        case 1:
        // -
            return (iOperand1-iOperand2);
        case 2:
        // x
            return (iOperand1*iOperand2);
        case 3:
        // /
            return (iOperand1/iOperand2);
    }
return 0;
}
void CFreeDialog::on_RemoveMark()
{
    ui->imgMark->setPixmap(QPixmap());
}
