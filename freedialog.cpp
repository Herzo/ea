#include <QMenu>
#include <QMessageBox>
#include <math.h>
#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <QEvent>

#include "freedialog.h"
#include "listgames.h"
#include "ui_freedialog.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include <unistd.h>
#include <X11/extensions/XTest.h>
extern "C" {
#include <xdo.h>
}


CFreeDialog::CFreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CFreeDialog)
{
    QSettings Settings;
    qsrand(time( NULL ));
    ui->setupUi(this);
    createActions();
    createTrayIcon();
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    m_pTrayIcon->show();
    InitIconStore();
    setIcon("Idle");
    // ui->GameMinutesCounter->display(qRound(Settings.value("gameminutes",0).toDouble()/100)*10);
    DisplayGameMinutes();
    ui->DifficultySlider->setValue(Settings.value("difficulty",10).toInt());
    InitQuestion();
    ui->Answer->installEventFilter(this);
    m_pTimer =new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(on_Update()));
    m_pTimer->start(500);
    InitSkin();
}

CFreeDialog::~CFreeDialog()
{
    delete m_pTimer;
    delete ui;
}


bool CFreeDialog::eventFilter(QObject *watched, QEvent *event)
{
     if (event->type() == 2)  // QEvent::KeyPress==2 namespace clash with xdo
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

//void CFreeDialog::on_pushButton_clicked()
//{
//    QPixmap imgTick(":/images/tick.png");
//    ui->imgMark->setPixmap(imgTick);
//}

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
//void CFreeDialog::on_buttonBox_accepted()
//{

//}

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

    m_pChangeSkin = new QAction(tr("&Change Skin"), this);
    connect(m_pChangeSkin, SIGNAL(triggered()), this, SLOT(ChangeSkin()));

    m_pIdentifyGames = new QAction(tr("&Identify Games"), this);
    connect(m_pIdentifyGames, SIGNAL(triggered()), this, SLOT(IdentifyGames()));
}
void CFreeDialog::ChangeSkin()
{
    QString sSkin;
    QSettings Settings;
    sSkin=Settings.value("skin",":/images/davesaveworld.png").toString();
    if(sSkin==":/images/davesaveworld.png")
        sSkin=":/images/rainboxworld.png";
    else
        sSkin=":/images/davesaveworld.png";
    Settings.setValue("skin",sSkin);
    InitSkin();
}

void CFreeDialog::IdentifyGames()
{
    CListGames dl;
    dl.exec();
}
void CFreeDialog::InitSkin()
{
    QSettings Settings;
    QPixmap img(Settings.value("skin",":/images/davesaveworld.png").toString());
    ui->Skin->setPixmap(img);
    ui->ClosedEye->hide();
}

void CFreeDialog::createTrayIcon()
{
    m_pTrayIconMenu = new QMenu(this);
    m_pTrayIconMenu->addAction(m_pMinimizeAction);
    m_pTrayIconMenu->addAction(m_pMaximizeAction);
    m_pTrayIconMenu->addAction(m_pRestoreAction);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pChangeSkin);
    m_pTrayIconMenu->addAction(m_pIdentifyGames);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pQuitAction);

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
}

void CFreeDialog::on_NextButton_clicked()
{
    // Check if they have answered correctly
    QSettings Settings;
    double dTotalGameMinutesCounter = Settings.value("totalgameminutes",0).toDouble();
    double dGameMinutesCounter = Settings.value("gameminutes",0).toDouble();
    int iAnswer=ui->Answer->toPlainText().toInt();
    double dQuestionsGameMinutes=GetQuestionsGameMinutes();
    // int iDifficultyBonus=(dTotalGameMinutesCounter/30000000.0)*1000; //

    if(iAnswer==m_iAnswer)
    {
        // They answered correctly
        QPixmap imgTick(":/images/tick.svg");
        ui->imgMark->setPixmap(imgTick);
        dTotalGameMinutesCounter+=dQuestionsGameMinutes;
        dGameMinutesCounter+=dQuestionsGameMinutes;
        if((qrand() % 10)==0)
        {
            ui->DifficultySlider->setValue(ui->DifficultySlider->value()+1);
        }
        InitQuestion();
    }else
    {
        QPixmap imgTick(":/images/cross.svg");
        ui->imgMark->setPixmap(imgTick);
        ui->Answer->setPlainText(QString::number(iAnswer));
        dTotalGameMinutesCounter-=dQuestionsGameMinutes;
        dGameMinutesCounter-=dQuestionsGameMinutes;
        if((qrand() % 2)==0)
        {
            ui->DifficultySlider->setValue(ui->DifficultySlider->value()-1);
        }
    }
    QTimer::singleShot(1500, this, SLOT(on_RemoveMark()));
    ui->Answer->setFocus();

    Settings.setValue("totalgameminutes",dTotalGameMinutesCounter);
    Settings.setValue("gameminutes",dGameMinutesCounter);
    Settings.setValue("difficulty",ui->DifficultySlider->value());
    DisplayGameMinutes();
}
void CFreeDialog::InitQuestion()
{
    // Generate a new question, and update the dialog
    ui->SecondsRemaining->setValue(119);
    int iDifficulty=ui->DifficultySlider->value();
    int iOperand1=(qrand() % iDifficulty); //  (qrand() % (max-min)+1) + min;
    int iOperand2=(qrand() % iDifficulty); // if min is 0 then just use max + 1
    int iOperand3=(qrand() % iDifficulty);
    int iMaxOperators=4;
    m_dQuestionDifficultyMultiplier=1;
    if(iDifficulty<60)
    {
        iMaxOperators=2;
    }
    if(iDifficulty<20)
    {
        iMaxOperators=1;
    }
    int iMaxPositions=3;
    if(iDifficulty<80)
    {
        iMaxPositions=2;
    }
    if(iDifficulty<40)
    {
        iMaxPositions=1;
        if(iOperand1<iOperand2)
            qSwap<int>(iOperand1,iOperand2);
    }
    int iOperator=(qrand() % iMaxOperators);
    int iLocationOfUnknown=2-(qrand() % iMaxPositions);
    qDebug() << iOperator << " "<< iLocationOfUnknown;
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
            m_dQuestionDifficultyMultiplier+=0.05;
        break;
        case 2:
            sOperator="x";
             iOperand1=(qrand() % 11)+1;
             iOperand2=(qrand() % 11)+1;
             iOperand3=iOperand1*iOperand2;
             m_dQuestionDifficultyMultiplier+=0.15;
        break;
        case 3:
            sOperator="/";
            iOperand2=(qrand() % 11)+1;
            iOperand3=(qrand() % 11)+1;
            iOperand1=iOperand2*iOperand3;
            m_dQuestionDifficultyMultiplier+=0.25;
    }
    // If any of the operands are negitive, then we increase m_dQuestionDifficulty
    if(iOperand1<0||iOperand2<0||iOperand3<0) // neg operands
        m_dQuestionDifficultyMultiplier+=0.05;
    if(iOperand1>99||iOperand2>99||iOperand3>99) // large operands
        m_dQuestionDifficultyMultiplier+=0.05;

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
            m_dQuestionDifficultyMultiplier+=0.15;
        break;
        case 1:
        // 6 + ? = 10
            ui->Operand2->setText("?");
            m_iAnswer=iOperand2;
            m_dQuestionDifficultyMultiplier+=0.15;
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

void CFreeDialog::on_DifficultySlider_valueChanged(int value)
{
    int iDifficulty=ui->DifficultySlider->value();
    QString sDifficultySign="Legend";
    if(iDifficulty<85)
    {
        sDifficultySign="Master";
    }
    if(iDifficulty<60)
    {
        sDifficultySign="Expert";
    }
    if(iDifficulty<40)
    {
        sDifficultySign="Advanced";
    }
    if(iDifficulty<20)
    {
        sDifficultySign="Easy";
    }
    ui->DifficultySign->setText(sDifficultySign);
}
void CFreeDialog::on_Update()
{
    // Called every half second by a qTimer
    int iSecondsRemaining=ui->SecondsRemaining->value()-1;
    if(iSecondsRemaining<0)
        iSecondsRemaining=0;
    ui->SecondsRemaining->setValue(iSecondsRemaining);
    ui->QuestionsGameMinutes->display(QString("%1").arg((GetQuestionsGameMinutes()/1000), 0, 'f', 3));
    // Animate dogs eye

    if((qrand() % 40)==0)
    {
        on_ControlGames();

        QString sSkin;
        QSettings Settings;
        sSkin=Settings.value("skin",":/images/davesaveworld.png").toString();
        if(sSkin==":/images/rainboxworld.png")
        {
            // Open eye
            ui->ClosedEye->hide();
            QTimer::singleShot(1500, this, SLOT(on_CloseEye()));

        }
    }
    // About once per minute, throw a thread off to check if a game is running, and if so, then deduct game minutes accordlingly.
    // Also, remember to update the status icons
    // If a game is running and we run out of game minutes, then exit the game.
}
void CFreeDialog::on_CloseEye()
{
    ui->ClosedEye->show();
}

void CFreeDialog::DisplayGameMinutes()
{
    QSettings Settings;
    double dGameMinutes=qRound(Settings.value("gameminutes",0).toDouble()/100.0)/10.0;
    ui->GameMinutesCounter->display(QString("%1").arg(dGameMinutes, 0, 'f', 1));
}
double CFreeDialog::GetQuestionsGameMinutes()
{
    // Adjust minutes earnt based on the difficutly, which is based on totalgameminutes, and
    // the time that it took to answer
    double dTimeBonusMultiplier=1+(ui->SecondsRemaining->value()/120.0);
    double dQuestionsGameMinutes=1000*m_dQuestionDifficultyMultiplier*dTimeBonusMultiplier;
    return qRound(1000*dQuestionsGameMinutes)/1000;
}
void CFreeDialog::on_ControlGames()
{
    xdo_t* xdo;
    XID ulWindowId=0;

    xdo = xdo_new(NULL);
    xdo->close_display_when_freed=true;
    xdo_get_active_window(xdo, &ulWindowId);

    unsigned char *name=0;
    int name_len=0;
    int name_type=0;

    xdo_get_window_name(xdo, ulWindowId, &name, &name_len, &name_type);

    QString sName=QString::fromStdString(std::string(reinterpret_cast<char*>(name)));
    qDebug() << ulWindowId << sName;
    if(sName=="Ubuntu Software Centre" || sName=="Minecraft 1.8.8")
    {
        // xdo_close_window(xdo, ulWindowId);
        xdo_kill_window(xdo, ulWindowId);
    }

    xdo_free(xdo);
}

