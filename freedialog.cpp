#include <QMenu>
#include <QMessageBox>
#include <math.h>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QSettings>
#include <QEvent>
#include <QShortcut>
#include <QMenuBar>
#include <QLayout>
#include <QUuid>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QVariantMap>
#include <QVariantList>
#include <QJsonArray>
#include <QSharedMemory>
#include <set>

#include "splashdialog.h"
#include "json.h"
#include "freedialog.h"
#include "listgames.h"
#include "listeducationals.h"
#include "ui_freedialog.h"

#if defined Q_OS_ANDROID
#elif defined Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include <unistd.h>
#include <X11/extensions/XTest.h>
// #include "xdo.h"
extern "C"
{
#include <xdo.h>
}
#elif defined Q_OS_WIN
#include <windows.h>
#elif defined Q_OS_MACX
    #import "Cocoa/Cocoa.h"
#elif defined Q_OS_IOS
// #include <dlfcn.h>
// #import <sys/sysctl.h>
// #import <CGWindow.h>
   // #import <Cocoa.h>
   // #import "ApplicationServices/CGWindow.h"
// #import <Foundation/Foundation.h>
// #import "QuartzCore/QuartzCore.h"
// #import <CoreFoundation/CoreFoundation.h>
// #import <Carbon/Carbon.h>
// #import <Quartz/Quartz.h>

// #import "ApplicationServices/ApplicationServices.h"
    #import "UIKit/UIKit.h"
#endif

CFreeDialog::CFreeDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::CFreeDialog)
{
    m_iMouseIdleCounter=0;
    m_GamingTimer.invalidate();
    QSettings Settings;
    qsrand(QTime::currentTime().msec());
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/emc2icon.ico"));
    createActions();
#if not defined Q_OS_IOS && not defined Q_OS_ANDROID
    createTrayIcon();
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    m_pTrayIcon->show();
#endif
    ui->Answer->installEventFilter(this);
    InitIconStore();
    setIcon("Emc2");
    DisplayGameMinutes();
    ui->DifficultySlider->setValue(Settings.value("difficulty", 10).toInt());
    InitQuestion();
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(on_Update()));
    m_pTimer->start(500);
    on_InitSkin();
    QTimer::singleShot(3000, this, SLOT(on_ControlGames()));
    QTimer::singleShot(200, this, SLOT(on_FetchGameFingerPrints()));
    QTimer::singleShot(600, this, SLOT(on_FetchEducationalFingerPrints()));
    if(Settings.value("showsplash",true).toBool()==true)
        QTimer::singleShot(50, this, SLOT(on_ShowSplash()));

    //    ui->SecondsRemaining->setWhatsThis(tr("<html><head/><body><p>This is the amont "
    //                                          "of speed bonus remaining on this question. </p><p>More challenging questions "
    //                                          "start with a greater time bonus.</p></body></html>"));

}
void CFreeDialog::on_ShowSplash()
{
    CSplashDialog* pDlg=new CSplashDialog(this);
    pDlg->show();
    pDlg->raise();

}

CFreeDialog::~CFreeDialog()
{
    ui->Answer->removeEventFilter(this);
    delete m_pTimer;
    delete ui;
}
// connect(this, SIGNAL(mySignal()), qApp, SLOT(aboutQt()));

bool CFreeDialog::eventFilter(QObject *watched, QEvent *event)
{
    // qDebug() << "Event Type: "<< event->type();
    if (event->type() == 6) // QEvent::KeyPress==6 namespace clash with xdo
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
        // qDebug() << "Event Key: "<< keyEvent->key();

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            on_NextButton_clicked();
            // QTimer::singleShot(0, this, SLOT(on_NextButton_clicked()));
        } else
        {
            return QObject::eventFilter(watched, event);
        }
        return true;
    } else
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
    // FIXME: We still need to activate the appropriate icon according to the state that we are in.
    m_IconStore.insert("Emc2", std::pair< QIcon, QString>(QIcon(":/images/emc2icon.ico"), "Einstein's Agent"));
    m_IconStore.insert("Idle", std::pair< QIcon, QString>(QIcon(":/images/idle.svg"), "No running games"));
    m_IconStore.insert("Running", std::pair< QIcon, QString>(QIcon(":/images/running.svg"), "Using game minutes"));
    m_IconStore.insert("Warning", std::pair< QIcon, QString>(QIcon(":/images/warning.svg"), "Game minutes are running low"));
    m_IconStore.insert("Bypass", std::pair< QIcon, QString>(QIcon(":/images/bypass.svg"), "Not monitoring or regulating game activity"));
    m_IconStore.insert("Blocking", std::pair< QIcon, QString>(QIcon(":/images/blocking.svg"), "Game minutes exhausted"));

}

void CFreeDialog::setVisible(bool visible)
{
    m_pMinimizeAction->setEnabled(visible);
    m_pMaximizeAction->setEnabled(!isMaximized());
    m_pRestoreAction->setEnabled(true);
    // m_pRestoreAction->setEnabled(isMaximized() || !visible);
#if not defined Q_OS_IOS && not defined Q_OS_ANDROID
    m_pTrayIcon->setVisible(true);
#endif
    QDialog::setVisible(visible);
}

void CFreeDialog::closeEvent(QCloseEvent *event)
{
#if not defined Q_OS_IOS && not defined Q_OS_ANDROID
    if (m_pTrayIcon->isVisible())
    {
        //        QMessageBox::information(this, tr("Einstein's Agent"),
        //                                 tr("Einstein's Agent will keep running in the "
        //                                    "system tray. To terminate the program, "
        //                                    "choose <b>Quit</b> in the context menu "
        //                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
#endif
}

void CFreeDialog::setIcon(QString sMode)
{
#if not defined Q_OS_IOS && not defined Q_OS_ANDROID
    QIcon icon = m_IconStore[sMode].first;
    m_pTrayIcon->setIcon(icon);
    setWindowIcon(icon);

    m_pTrayIcon->setToolTip(m_IconStore[sMode].second);
#endif
}

void CFreeDialog::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        setVisible(true);
      //  this->raise();
      //  this->activateWindow();
      //  this->showNormal();

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
    connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(quit()));

    m_pChangeSkin = new QAction(tr("&Change Skin"), this);
    connect(m_pChangeSkin, SIGNAL(triggered()), this, SLOT(ChangeSkin()));

    m_pIdentifyGames = new QAction(tr("&Identify Games"), this);
    connect(m_pIdentifyGames, SIGNAL(triggered()), this, SLOT(IdentifyGames()));

    m_pIdentifyEducationals = new QAction(tr("&Identify Educationals"), this);
    connect(m_pIdentifyEducationals, SIGNAL(triggered()), this, SLOT(IdentifyEducationals()));
}
void CFreeDialog::quit()
{
#if defined Q_OS_WIN
    /*
    // Implement genWin32ShellExecute() especially for UAC
    // on windows pop up the UAC to make sure that the user knows the admin password
    QString AppToExec = qApp->applicationDirPath() + "/Einstein.exe";
    // Put any required parameters of App2.exe to AppParams string
    QString AppParams = "";
    if (0 != genWin32ShellExecute(AppToExec,
                                  "runas",    // default verb: "open" or "exec"
                                  AppParams,
                                  false, // run hidden
                                  true)) // wait to finish
    {
        // (...) handle error
        return; // They probably did not enter the admin password correctly.
    }
    */
#endif
    qApp->quit();
}

void CFreeDialog::ChangeSkin()
{
    QString sSkin;
    QString sSkinGameSelection;
    QString sMinutesWarning;
    QString sSkinGetWindowTitle;
    QSettings Settings;
    sSkin = Settings.value("skin", ":/images/davesaveworld.png").toString();
    sSkinGameSelection = Settings.value("skinlistgames", ":/images/gameselectionherzo.png").toString();
    if (sSkin == ":/images/davesaveworld.png")
    {
        sSkin = ":/images/rainboxworld.png";
        sSkinGameSelection = ":/images/gameselectionherzoine.png";
        sMinutesWarning = ":/images/hellooo.png";
        sSkinGetWindowTitle = ":/images/narniethumb101x161.png";
    } else
    {
        sSkin = ":/images/davesaveworld.png";
        sSkinGameSelection = ":/images/gameselectionherzo.png";
        sMinutesWarning = ":/images/davelike.png";
        sSkinGetWindowTitle = ":/images/Dave_106x150.png";
    }
    Settings.setValue("skin", sSkin);
    Settings.setValue("skinlistgames", sSkinGameSelection);
    Settings.setValue("skinlisteducationals", sSkinGameSelection);
    Settings.setValue("minuteswarning", sMinutesWarning);
    Settings.setValue("skingetwindowtitle",sSkinGetWindowTitle);
    on_InitSkin();
}

void CFreeDialog::IdentifyGames()
{
    CListGames dl;
    dl.exec();
}

void CFreeDialog::IdentifyEducationals()
{
    CListEducationals dl;
    dl.exec();
}

void CFreeDialog::on_InitSkin()
{
    QSettings Settings;
    QPixmap img(Settings.value("skin", ":/images/davesaveworld.png").toString());
    ui->Skin->setPixmap(img);
    ui->ClosedEye->hide();
    QPixmap imgMinutesWarning(Settings.value("minuteswarning", ":/images/davelike.png").toString());
    ui->MinutesWarning->setPixmap(imgMinutesWarning);
    ui->MinutesWarning->hide();
    ui->pushButtonGameMinOK->hide();
}
#if not defined Q_OS_IOS && not defined Q_OS_ANDROID

void CFreeDialog::createTrayIcon()
{
    m_pTrayIconMenu = new QMenu(this);
    m_pTrayIconMenu->addAction(m_pMinimizeAction);
    m_pTrayIconMenu->addAction(m_pMaximizeAction);
    m_pTrayIconMenu->addAction(m_pRestoreAction);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pChangeSkin);
    m_pTrayIconMenu->addAction(m_pIdentifyGames);
    m_pTrayIconMenu->addAction(m_pIdentifyEducationals);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pQuitAction);

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
}
#endif
void CFreeDialog::on_NextButton_clicked()
{
    // Check if they have answered correctly
    QSettings Settings;
    uint dTotalGameMillisecondsCounter = Settings.value("totalgamemilliseconds", 0).toUInt();
    uint dGameMillisecondsCounter = Settings.value("gamemilliseconds", 0).toUInt();
    if (ui->Answer->toPlainText() == "")
        return;
    int iAnswer = ui->Answer->toPlainText().toInt();
    uint dQuestionsGameMilliseconds = GetQuestionsGameMilliseconds();

    if (iAnswer == m_iAnswer)
    {
        // They answered correctly
        QPixmap imgTick(":/images/tick.svg");
        ui->imgMark->setPixmap(imgTick);
        dTotalGameMillisecondsCounter += dQuestionsGameMilliseconds;
        dGameMillisecondsCounter += dQuestionsGameMilliseconds;
        if ((qrand() % 10) == 0)
        {
            ui->DifficultySlider->setValue(ui->DifficultySlider->value() + 1);
        }
        InitQuestion();
        setIcon("Idle");
    } else
    {
        QPixmap imgTick(":/images/cross.svg");
        ui->imgMark->setPixmap(imgTick);
        ui->Answer->setPlainText(QString::number(iAnswer));
        if (dQuestionsGameMilliseconds > dGameMillisecondsCounter)
        {
            dGameMillisecondsCounter = 0;
        } else
        {
            dGameMillisecondsCounter -= dQuestionsGameMilliseconds;
        }
        if ((qrand() % 2) == 0)
        {
            ui->DifficultySlider->setValue(ui->DifficultySlider->value() - 1);
        }
    }
    QTimer::singleShot(1500, this, SLOT(on_RemoveMark()));
    ui->Answer->setFocus();

    Settings.setValue("totalgamemilliseconds", dTotalGameMillisecondsCounter);
    Settings.setValue("gamemilliseconds", dGameMillisecondsCounter);
    Settings.setValue("difficulty", ui->DifficultySlider->value());
    DisplayGameMinutes();
}

void CFreeDialog::InitQuestion()
{
    // Generate a new question, and update the dialog
    int iDifficulty = ui->DifficultySlider->value();
    int iOperand1 = (qrand() % iDifficulty); //  (qrand() % (max-min)+1) + min;
    int iOperand2 = (qrand() % iDifficulty); // if min is 0 then just use max + 1
    int iOperand3 = (qrand() % iDifficulty);
    int iMaxOperators = 4;
    m_dQuestionDifficultyMultiplier = 1;
    if (iDifficulty < 60)
    {
        iMaxOperators = 2;
    }
    if (iDifficulty < 20)
    {
        iMaxOperators = 1;
    }
    int iMaxPositions = 3;
    if (iDifficulty < 80)
    {
        iMaxPositions = 2;
    }
    if (iDifficulty < 40)
    {
        iMaxPositions = 1;
        if (iOperand1 < iOperand2)
            qSwap<int>(iOperand1, iOperand2);
    }
    int iOperator = (qrand() % iMaxOperators);
    int iLocationOfUnknown = 2 - (qrand() % iMaxPositions);
    // qDebug() << iOperator << " "<< iLocationOfUnknown;
    QString sOperator = "+";
    switch (iOperator)
    {
    case 0:
        sOperator = "+";
        iOperand3 = iOperand1 + iOperand2;
        break;
    case 1:
        sOperator = "-";
        iOperand3 = iOperand1 - iOperand2;
        m_dQuestionDifficultyMultiplier += 0.05;
        break;
    case 2:
        sOperator = "x";
        iOperand1 = (qrand() % 11) + 1;
        iOperand2 = (qrand() % 11) + 1;
        iOperand3 = iOperand1*iOperand2;
        m_dQuestionDifficultyMultiplier += 0.15;
        break;
    case 3:
        sOperator = "/";
        iOperand2 = (qrand() % 11) + 1;
        iOperand3 = (qrand() % 11) + 1;
        iOperand1 = iOperand2*iOperand3;
        m_dQuestionDifficultyMultiplier += 0.25;
    }
    // If any of the operands are negitive, then we increase m_dQuestionDifficulty
    if (iOperand1 < 0 || iOperand2 < 0 || iOperand3 < 0) // neg operands
        m_dQuestionDifficultyMultiplier += 0.10;
    if (iOperand1 > 99 || iOperand2 > 99 || iOperand3 > 99) // large operands
        m_dQuestionDifficultyMultiplier += 0.05;

    ui->Operand1->setText(QString::number(iOperand1));
    ui->Operand2->setText(QString::number(iOperand2));
    ui->Operand3->setText(QString::number(iOperand3));
    ui->Operator->setText(sOperator);
    switch (iLocationOfUnknown)
    {
    case 0:
        // ? + 4 = 10
        ui->Operand1->setText("?");
        m_iAnswer = iOperand1;
        m_dQuestionDifficultyMultiplier += 0.15;
        break;
    case 1:
        // 6 + ? = 10
        ui->Operand2->setText("?");
        m_iAnswer = iOperand2;
        m_dQuestionDifficultyMultiplier += 0.15;
        break;
    case 2:
        // 6 + 4 = ?
        ui->Operand3->setText("?");
        m_iAnswer = iOperand3;
    }
    ui->Answer->setPlainText("");
    ui->SecondsRemaining->setValue(59 * m_dQuestionDifficultyMultiplier);
}

void CFreeDialog::on_RemoveMark()
{
    ui->imgMark->setPixmap(QPixmap());
}

void CFreeDialog::on_DifficultySlider_valueChanged(int value)
{
    // int iDifficulty = ui->DifficultySlider->value();
    int iDifficulty = value;
    QString sDifficultySign = "Legend";
    if (iDifficulty < 85)
    {
        sDifficultySign = "Master";
    }
    if (iDifficulty < 60)
    {
        sDifficultySign = "Expert";
    }
    if (iDifficulty < 40)
    {
        sDifficultySign = "Advanced";
    }
    if (iDifficulty < 20)
    {
        sDifficultySign = "Easy";
    }
    ui->DifficultySign->setText(sDifficultySign);
}

void CFreeDialog::on_Update()
{
    // Called every half second by a qTimer
    // Check for a singal from a new instance of ourselves
    QSharedMemory mem("einstein");
    if(mem.attach())
    {
        void* pData=mem.data();
        quint64 &uiData = *(quint64*)(pData);
        if(uiData==1)
        {
            uiData=0;  // signal back that we are already here
            this->raise();
            this->activateWindow();
            this->showNormal();
        }
    }

    int iSecondsRemaining = ui->SecondsRemaining->value() - 1;
    if (iSecondsRemaining < 0)
        iSecondsRemaining = 0;
    ui->SecondsRemaining->setValue(iSecondsRemaining);
    DisplayQuestionGameMinutes();
    // Animate dogs eye
    static int iCounter = 19;
    //    if((qrand() % 20)==0)
    if (iCounter-- == 0)
    {
        iCounter = 19;
        on_ControlGames();

        QString sSkin;
        QSettings Settings;
        sSkin = Settings.value("skin", ":/images/davesaveworld.png").toString();
        if (sSkin == ":/images/rainboxworld.png")
        {
            // Open eye
            ui->ClosedEye->hide();
            QTimer::singleShot(1500, this, SLOT(on_CloseEye()));

        }
    }

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
    uint uiGameSeconds = Settings.value("gamemilliseconds").toUInt() / 1000;
    uint iSeconds = uiGameSeconds % 60;
    uint iMinutes = uiGameSeconds / 60;
    QString sDisplay = QString("%1:%2")
        .arg(iMinutes).arg(QString::number(iSeconds).rightJustified(2, '0'));
    ui->GameMinutesCounter->display(sDisplay);
    //double dGameMinutes=qRound(Settings.value("gamemilliseconds",0).toDouble()/100.0)/10.0;
    //ui->GameMinutesCounter->display(QString("%1").arg(dGameMinutes, 0, 'f', 1));
}

void CFreeDialog::DisplayQuestionGameMinutes()
{

    int uiQuestionsGameSeconds = GetQuestionsGameMilliseconds() / 1000;
    int iSeconds = uiQuestionsGameSeconds % 60;
    int iMinutes = uiQuestionsGameSeconds / 60;
    QString sDisplay = QString("%1:%2")
        .arg(iMinutes).arg(QString::number(iSeconds).rightJustified(2, '0'));
    ui->QuestionsGameMinutes->display(sDisplay);
    // ui->QuestionsGameMinutes->display(QString("%1").arg((GetQuestionsGameMilliseconds()/1000), 0, 'f', 3));

}

uint CFreeDialog::GetQuestionsGameMilliseconds()
{
    // Adjust minutes earnt based on the difficutly, which is based on totalGameMilliseconds, and
    // the time that it took to answer
    double dTimeBonusMultiplier = 1 + (ui->SecondsRemaining->value() / 60.0);
    double dQuestionsGameMilliseconds = 15000 * m_dQuestionDifficultyMultiplier*dTimeBonusMultiplier; // 15000 == 15 sec
    return dQuestionsGameMilliseconds;
    // return qRound(1000*dQuestionsGameMilliseconds)/1000;
}

void CFreeDialog::on_FetchGameFingerPrints()
{
    QSettings Settings;
    QString sAfter = Settings.value("MaxGamesFingerPrintId", "0").toString();
    // Get the highest game id that we know of
    // QString sMaxGameId = Settings.value("MaxGameId").toString();
    QNetworkAccessManager *p_NetworkAccessManager = new QNetworkAccessManager(this);
    connect(p_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(on_ServerReply(QNetworkReply*)));

    QString sUrl = QString("http://einstein.geobytes.com/GetGameFingerPrints?ver=%1&uuid=%2&afterid=%3")
        .arg(CFreeDialog::GetVersion())
        .arg(GetUuId())
        .arg(sAfter);
    p_NetworkAccessManager->get(QNetworkRequest(QUrl(sUrl)));
}
QString CFreeDialog::GetUuId()
{
    QSettings Settings;
    QString sUuId = Settings.value("UuId", "notset").toString();

    if (sUuId == "notset"||sUuId.contains("{"))
    {
        // generate a new UUID and save it in settings.
        sUuId = QUuid::createUuid().toString();
        sUuId.replace("{","");
        sUuId.replace("}","");
        Settings.setValue("UuId", sUuId);
    }
    return sUuId;
}
void CFreeDialog::on_FetchEducationalFingerPrints()
{
    QSettings Settings;
    QString sAfter = Settings.value("MaxEducationalsFingerPrintId", "0").toString();
    // Get the highest game id that we know of
   // QString sMaxGameId = Settings.value("MaxGameId").toString();
    QNetworkAccessManager *p_NetworkAccessManager = new QNetworkAccessManager(this);
    connect(p_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(on_ServerReply(QNetworkReply*)));

    QString sUrl = QString("http://einstein.geobytes.com/GetEducationalFingerPrints?ver=%1&uuid=%2&afterid=%3")
        .arg(CFreeDialog::GetVersion())
        .arg(GetUuId())
        .arg(sAfter);
    p_NetworkAccessManager->get(QNetworkRequest(QUrl(sUrl)));
}

void CFreeDialog::on_ServerReply(QNetworkReply* pReply)
{
    if (pReply->error() != QNetworkReply::NoError)
        return;
    QString data = (QString) pReply->readAll();
    // data should look something like
    // {
    //  "Request": "GetGameFingerPrints",
    //  "MaxFingerPrintId": "23",
    //  "FingerPrints": [
    //    "minecraft",
    //    "solitaire"
    //  ]
    // }

    // QtJson::parse parser;
    // QtJson::JsonObject result = QtJson::parse(data, ok).toMap();

    bool ok;
    QVariantMap ResponseMap = QtJson::parse(data, ok).toMap();
    if (ok)
    {
        // Read the existing list from settings
        QSettings settings;
        QString sRequest=ResponseMap["Request"].toString();
        QString sMaxFingerPrintId=ResponseMap["MaxFingerPrintId"].toString();
        QString sArrayName="educationals";
        if(sRequest=="GetGameFingerPrints")
        {
            sArrayName="games";
        }else
        {
            sArrayName="educationals";
        }
        settings.setValue(QString("Max%1FingerPrintId").arg(sArrayName),sMaxFingerPrintId);
        // Read the existing list from settings, and append them to ExistingGamesList
        // Write the list in to a set first to remove the dups
        std::set<QString> Items;
        int iSize = settings.beginReadArray(sArrayName); //Get the current size
        for (int i = 0; i < iSize; ++i)
        {
            settings.setArrayIndex(i);
            QString sWindowTitle = settings.value("windowtitle").toString();
            if(sWindowTitle.trimmed()!="")
                Items.insert(sWindowTitle);
        }
        settings.endArray();
        // Read the new titles list from the server's response, and append them to ExistingGamesList
        QVariantList FingerPrints=ResponseMap["FingerPrints"].toList();
        if (ok)
        {
            for (int i = 0; i < FingerPrints.size(); ++i)
            {
                QString sWindowTitle = FingerPrints[i].toString();
                if(sWindowTitle.trimmed()!="")
                        Items.insert(sWindowTitle);
            }
        }
        // Write the combined list to settings
        settings.beginWriteArray(sArrayName);
        int iElement=0;
        std::set<QString>::const_iterator it=Items.begin();
        while (it!=Items.end())
        {
            settings.setArrayIndex(iElement);
            settings.setValue("windowtitle", (*it));
            iElement++;
            it++;
        }
        settings.endArray();
    }


    // QDateTime::currentTime().toString("yyyy-MM-dd%20hh:mm:ss");
}
#if defined Q_OS_MACX || defined Q_OS_IOS

QString qt_mac_NSStringToQString(const NSString *nsstr)
{
    NSRange range;
    range.location = 0;
    range.length = [nsstr length];
    QString result(range.length, QChar(0));

    unichar *chars = new unichar[range.location];
    [nsstr getCharacters:chars range:range];
    result = QString::fromUtf16(chars, range.length);
    delete  chars;
    return result;
}
// #elif defined Q_OS_IOS
/*
 NSArray* getActiveApps()
 {
     int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
     size_t miblen = 4;

     size_t size;
     int st = sysctl(mib, miblen, NULL, &size, NULL, 0);

     struct kinfo_proc * process = NULL;
     struct kinfo_proc * newprocess = NULL;

     do {

         size += size / 10;
         newprocess = (struct kinfo_proc *) realloc(process, size);

         if (!newprocess){

             if (process){
                 free(process);
             }

             return nil;
         }

         process = newprocess;
         st = sysctl(mib, miblen, process, &size, NULL, 0);

     } while (st == -1 && errno == ENOMEM);

     if (st == 0){

         if (size % sizeof(struct kinfo_proc) == 0){
             int nprocess = size / sizeof(struct kinfo_proc);

             if (nprocess){

                 NSMutableArray * array = [[NSMutableArray alloc] init];

                 for (int i = nprocess - 1; i >= 0; i--){

                     NSString * processID = [[NSString alloc] initWithFormat:@"%d", process[i].kp_proc.p_pid];
                     NSString * processName = [[NSString alloc] initWithFormat:@"%s", process[i].kp_proc.p_comm];

                     NSDictionary * dict = [[NSDictionary alloc] initWithObjects:[NSArray arrayWithObjects:processID, processName, nil]
                                                                         forKeys:[NSArray arrayWithObjects:@"ProcessID", @"ProcessName", nil]];
                     QString sName = qt_mac_NSStringToQString(processName);
                     qDebug() << sName;
                     [processID release];
                     [processName release];
                     [array addObject:dict];
                     [dict release];
                 }

                 free(process);
                 return [array autorelease];
             }
         }
     }

     return nil;
 }
 */
#endif
void CFreeDialog::on_ControlGames()
{
    /*
     *
 // link against libX11

     * */
    // Deduce the game minutes used since the last check
    // Check if we have any game minutes avalible
    // if we are out of game minutes then minimise the game
    if(m_pointLastMousePos==QCursor::pos())
    {
        m_iMouseIdleCounter++;
    }else
    {
        m_iMouseIdleCounter=0;
        m_pointLastMousePos=QCursor::pos();
    }
    QSettings Settings;
    QString sName;
#if defined Q_OS_ANDROID

#elif defined Q_OS_LINUX

    unsigned char *name = 0;
    int name_len = 0;
    int name_type = 0;

    xdo_t* xdo;
    XID ulWindowId = 0;

    xdo = xdo_new(NULL);
    if (xdo == NULL)
    {
        qDebug() << "xdo could not get a new display";
        return;
    }
    xdo->close_display_when_freed = true;
    xdo_get_active_window(xdo, &ulWindowId);
    xdo_get_window_name(xdo, ulWindowId, &name, &name_len, &name_type);
    if (name == NULL)
    {
        qDebug() << "xdo could not get window name";
    } else
    {
        sName = QString::fromStdString(std::string(reinterpret_cast<char*> (name)));
    }
    XFree(name); // need to add -lX11 to LIBS in project
    xdo_free(xdo);

    if (sName == "")
        return;

#elif defined Q_OS_WIN
    wchar_t wnd_title[512];
    HWND hwnd = GetForegroundWindow(); // get handle of currently active window
    if (hwnd != NULL)
        GetWindowText(hwnd, wnd_title, sizeof (wnd_title));
    sName = QString::fromStdWString(std::wstring(wnd_title));
#elif defined Q_OS_MACX
    // https://forum.qt.io/topic/7761/getting-the-window-title-of-a-non-qt-application/8
    int i =0;
 //   NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSMutableArray *windows = (NSMutableArray *)CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements, kCGNullWindowID);

    for (NSDictionary *window in windows)
    {
        NSString *owner = [window objectForKey:@"kCGWindowOwnerName" ];
        NSString *name = [window objectForKey:@"kCGWindowName" ];
        NSNumber *layer = [window objectForKey:@"kCGWindowLayer"];

        if([layer intValue] == 0 && i == 0)
        {
            /* only returns the first window title.
            Window titles are returned are in order from front to back and i only want the
            frontmost active window title.*/
            NSString *title = [owner stringByAppendingString:@" - "];
            title = [title stringByAppendingString:name];
            sName = qt_mac_NSStringToQString(title);
            qDebug() << sName;
            i++;
        }
    }
   // [windows release];
#elif defined Q_OS_IOS
    // On iOS we can't see what other apps are running.
    // So we asume that other apps are all games.
    // However we should be able to tell when the device is asleep.
    //getActiveApps();
//   id<UIApplicationDelegate> appDelegate = [[UIApplication sharedApplication] delegate];
//   UIWindow * mainWindow = [[[UIApplication sharedApplication] windows] firstObject];

   //if the process of the current window in the list matches our process, get the front window number
   //    if(myPSN.lowLongOfPSN == currentAppPSN.lowLongOfPSN && myPSN.highLongOfPSN == currentAppPSN.highLongOfPSN)
     //  {
   // CGWindowListOption option=0,
    // CGWindowID relativeToWindow=0;
    /*
     * You can detect when your app stops being the active application with:

- (void)applicationDidEnterBackground:(UIApplication *)application
in the app delegate.

Also, you can detect when it comes back into view with:

- (void)applicationDidBecomeActive:(UIApplication *)application

    enum
    {
       kCGWindowListOptionAll                 = 0,
       kCGWindowListOptionOnScreenOnly        = (1 << 0),
       kCGWindowListOptionOnScreenAboveWindow = (1 << 1),
       kCGWindowListOptionOnScreenBelowWindow = (1 << 2),
       kCGWindowListOptionIncludingWindow     = (1 << 3),
       kCGWindowListExcludeDesktopElements    = (1 << 4)
    };
   CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, 0);
   for (NSMutableDictionary* entry in (NSArray*)windowList)
   {
       NSString* ownerName = [entry objectForKey:(id)kCGWindowOwnerName];
       NSInteger ownerPID = [[entry objectForKey:(id)kCGWindowOwnerPID] integerValue];
       NSLog(@"%@:%ld", ownerName, (long)ownerPID);
   }
   CFRelease(windowList);

   */
/*           NSNumber* windowNumber    = [mainWindow objectForKey:(id)kCGWindowNumber];
           NSString* applicationName = [mainWindow objectForKey:(id)kCGWindowOwnerName];
           NSLog(@"The current app is %@ and the window number of its front window is %@.",applicationName,windowNumber);
           CGRect bounds;
           CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)[entry objectForKey:(id)kCGWindowBounds], &bounds);
           NSLog(@"WINDOW RECT BOUNDS; (x,y,width, height) = (%d,%d, %d, %d)", bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);


           break;
           */
       //}

#endif

    // qDebug() << ulWindowId << sName;

    // Is the currently actitive window an educational?
    bool bPlayingEducational = false;
    int iSize = Settings.beginReadArray("educationals");
    for (int i = 0; i < iSize; ++i)
    {
        Settings.setArrayIndex(i);
        QString sEducationalWindowTitle = Settings.value("windowtitle").toString();
        // remove non alpha charaters
        // sName.simplified()
        if (sName.contains(sEducationalWindowTitle, Qt::CaseInsensitive))
        {
            // They are playing a Educational
            bPlayingEducational = true;
        }
    }
    Settings.endArray();


    // Is the currently actitive window a game?
    bool bPlayingGame = false;
    iSize = Settings.beginReadArray("games");
    for (int i = 0; i < iSize; ++i)
    {
        Settings.setArrayIndex(i);
        QString sGameWindowTitle = Settings.value("windowtitle").toString();
        // remove non alpha charaters
        // sName.simplified()
        if (sName.contains(sGameWindowTitle, Qt::CaseInsensitive))
        {
            // They are playing a game
            bPlayingGame = true;
        }
    }
    Settings.endArray();

    if (bPlayingEducational == true&&m_iMouseIdleCounter<3)
    {
        uint uiGameMillisecondsAvalable = Settings.value("gamemilliseconds", 0).toUInt();
        if (!m_GamingTimer.isValid())
        {
            m_GamingTimer.start();
            setIcon("Emc2");
            return;
        }
        uint ulGamingMilliseconds = m_GamingTimer.elapsed();
        uiGameMillisecondsAvalable = uiGameMillisecondsAvalable + (ulGamingMilliseconds*Settings.value("educationalminutesmultiplier",1).toInt());
        Settings.setValue("gamemilliseconds", uiGameMillisecondsAvalable);
        DisplayGameMinutes();
        m_GamingTimer.restart();
        setIcon("Emc2");
        return;
    }


    if (bPlayingGame == true)
    {
        uint uiGameMillisecondsAvalable = Settings.value("gamemilliseconds", 0).toUInt();
        if (uiGameMillisecondsAvalable > 0)
        {
            if (!m_GamingTimer.isValid())
            {
                m_GamingTimer.start();
                setIcon("Running");
                return;
            }
            uint ulGamingMilliseconds = m_GamingTimer.elapsed();
            if (uiGameMillisecondsAvalable > ulGamingMilliseconds)
            {
                uiGameMillisecondsAvalable = uiGameMillisecondsAvalable - ulGamingMilliseconds;
                Settings.setValue("gamemilliseconds", uiGameMillisecondsAvalable);
                DisplayGameMinutes();
                m_GamingTimer.restart();
                setIcon("Running");
                return;
            }
        }
        setIcon("Blocking");
        m_GamingTimer.invalidate();
        Settings.setValue("gamemilliseconds", 0);
        DisplayGameMinutes();
        // xdo_close_window(xdo, ulWindowId);
        // xdo_kill_window(xdo, ulWindowId);
#if defined Q_OS_ANDROID
#elif defined Q_OS_LINUX
        xdo = xdo_new(NULL);
        if (xdo == NULL)
        {
            qDebug() << "xdo could not get a new display on the second call";
        } else
        {
            xdo->close_display_when_freed = true;
            xdo_minimize_window(xdo, ulWindowId);
            // XFree(name); // need to add -lX11 to LIBS in project
            xdo_free(xdo);
        }
#elif defined Q_OS_WIN
        ShowWindow(hwnd, SW_FORCEMINIMIZE);
#elif defined Q_OS_MACX
//        setWindowState((windowState() & -Qt::WindowMinimized) |Qt::WindowActive);
//        raise();
//        activateWindow();
#endif
        // must auto-close dialog, as leaving it open blocks.
        this->raise();
        this->activateWindow();
        this->showNormal();
        ui->MinutesWarning->raise();
        ui->pushButtonGameMinOK->raise();
        ui->MinutesWarning->show();
        ui->pushButtonGameMinOK->show();
        QTimer::singleShot(12500, this, SLOT(on_InitSkin()));
    }
    m_GamingTimer.invalidate();
    setIcon("Idle");
}


void CFreeDialog::on_actionID_Games_triggered()
{
    IdentifyGames();

}

void CFreeDialog::on_actionChange_Skin_triggered()
{
    ChangeSkin();
}

void CFreeDialog::on_actionAbout_Einstein_s_Agent_triggered()
{
    // Use https://iconverticons.com/online/ to convert svg to ioc format
    QMessageBox::about(this, "Einstein's Agent", QString("Einstein's Agent %1 (opensource)\n\n"\
        "Copyright 2015 Geobytes, inc. All rights reserved.\n\n"\
        "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF "\
        "DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.").arg(CFreeDialog::GetVersion()));
}

void CFreeDialog::on_pushButtonGameMinOK_clicked()
{
    on_InitSkin();
}

void CFreeDialog::on_actionManage_Educationals_triggered()
{
    IdentifyEducationals();
}


void CFreeDialog::on_checkBoxMultiplication_clicked(bool checked)
{
    if(checked)
        m_MultiplicationDialog.show();
    else
        m_MultiplicationDialog.hide();
}

void CFreeDialog::on_checkBoxAddition_clicked(bool checked)
{
    if(checked)
        m_AdditionDialog.show();
    else
        m_AdditionDialog.hide();
}

void CFreeDialog::on_actionHelp_triggered()
{
    emit on_ShowSplash();
}

void CFreeDialog::on_btnChangeSkin_clicked()
{
    ChangeSkin();
}
// Execute/Open the specified Application/Document with the given command
// line Parameters
// (if WaitToFinish == true, wait for the spawn process to finish)
//
// Verb parameter values:
// ""           The degault verb for the associated AppFullPath
// "edit"       Launches an editor and opens the document for editing.
// "find"       Initiates a search starting from the specified directory.
// "open"       Launches an application. If this file is not an executable file, its associated application is launched.
// "print"      Prints the document file.
// "properties" Displays the object's properties.
//
// Ret: 0 = success
//     <0 = error
#if defined Q_OS_WIN
int CFreeDialog::genWin32ShellExecute(QString AppFullPath,
                         QString Verb,
                         QString Params,
                         bool ShowAppWindow,
                         bool WaitToFinish)
{
    int Result = 0;

    // Setup the required structure
    SHELLEXECUTEINFO ShExecInfo;
    memset(&ShExecInfo, 0, sizeof(SHELLEXECUTEINFO));
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    if (Verb.length() > 0)
        ShExecInfo.lpVerb = reinterpret_cast<const WCHAR *>(Verb.utf16());
    ShExecInfo.lpFile = NULL;
    if (AppFullPath.length() > 0)
        ShExecInfo.lpFile = reinterpret_cast<const WCHAR *>(AppFullPath.utf16());
    ShExecInfo.lpParameters = NULL;
    if (Params.length() > 0)
        ShExecInfo.lpParameters = reinterpret_cast<const WCHAR *>(Params.utf16());
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = (ShowAppWindow ? SW_SHOW : SW_HIDE);
    ShExecInfo.hInstApp = NULL;

    // Spawn the process
    if (ShellExecuteEx(&ShExecInfo) == FALSE)
    {
        Result = -1; // Failed to execute process
    } else if (WaitToFinish)
    {
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    }

    return Result;
}
#endif
