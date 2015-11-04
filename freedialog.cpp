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

#include "splashdialog.h"
#include "json.h"
#include "freedialog.h"
#include "listgames.h"
#include "listeducationals.h"
#include "ui_freedialog.h"

#ifndef WIN32
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
#else
#include <windows.h>
#endif

CFreeDialog::CFreeDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::CFreeDialog)
{
    m_iMouseIdleCounter=0;
    m_GamingTimer.invalidate();
    // m_ulLastGamingTime=0;
    QSettings Settings;
    qsrand(QTime::currentTime().msec());
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/emc2icon.ico"));
    createActions();
    createTrayIcon();
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    m_pTrayIcon->show();
    ui->Answer->installEventFilter(this);
//    ui->SecondsRemaining->setWhatsThis(tr("<html><head/><body><p>This is the amont "
//                                          "of speed bonus remaining on this question. </p><p>More challenging questions "
//                                          "start with a greater time bonus.</p></body></html>"));
    InitIconStore();
    setIcon("Emc2");
    // ui->GameMinutesCounter->display(qRound(Settings.value("gamemilliseconds",0).toDouble()/100)*10);
    DisplayGameMinutes();
    ui->DifficultySlider->setValue(Settings.value("difficulty", 10).toInt());
    InitQuestion();
    //ui->groupBox->installEventFilter(this);
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(on_Update()));
    m_pTimer->start(500);
    on_InitSkin();
    QTimer::singleShot(3000, this, SLOT(on_ControlGames()));
    QTimer::singleShot(200, this, SLOT(on_FetchGameFingerPrints()));
    QTimer::singleShot(600, this, SLOT(on_FetchEducationalFingerPrints()));
    if(Settings.value("showsplash").toBool()==true,true)
        QTimer::singleShot(50, this, SLOT(on_ShowSplash()));

        // pressing Enter activates the slots only when list widget has focus
    //    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Enter), ui->Answer);
    //    connect(shortcut, SIGNAL(activated()), this, SLOT(on_NextButton_clicked()));
    //    QShortcut* shortcut1 = new QShortcut(QKeySequence(Qt::Key_Return), ui->Answer);
    //    connect(shortcut, SIGNAL(activated()), this, SLOT(on_NextButton_clicked()));

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
    m_pRestoreAction->setEnabled(isMaximized() || !visible);
    m_pTrayIcon->setVisible(true);
    QDialog::setVisible(visible);
}

void CFreeDialog::closeEvent(QCloseEvent *event)
{
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
    switch (reason)
    {
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

    m_pIdentifyEducationals = new QAction(tr("&Identify Educationals"), this);
    connect(m_pIdentifyEducationals, SIGNAL(triggered()), this, SLOT(IdentifyEducationals()));
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
    int iDifficulty = ui->DifficultySlider->value();
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
        QVariantList ExistingGamesList;
        int iSize = settings.beginReadArray(sArrayName); //Get the current size
        for (int i = 0; i < iSize; ++i)
        {
            settings.setArrayIndex(i);
            QString sWindowTitle = settings.value("windowtitle").toString();
            ExistingGamesList.append(sWindowTitle);
        }
        settings.endArray();
        QVariantList FingerPrints=ResponseMap["FingerPrints"].toList();
        if (ok)
        {
            for (int i = 0; i < FingerPrints.size(); ++i)
            {
                QString sFingerPrint = FingerPrints[i].toString();
                if(!ExistingGamesList.contains(sFingerPrint))
                {
                    settings.beginWriteArray(sArrayName);
                    settings.setArrayIndex(iSize + i);
                    settings.setValue("windowtitle", sFingerPrint);
                    settings.endArray();
                }
            }
        }
    }
    // QDateTime::currentTime().toString("yyyy-MM-dd%20hh:mm:ss");
}

void CFreeDialog::on_ControlGames()
{
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
#ifndef WIN32

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

#else
    wchar_t wnd_title[512];
    HWND hwnd = GetForegroundWindow(); // get handle of currently active window
    if (hwnd != NULL)
        GetWindowText(hwnd, wnd_title, sizeof (wnd_title));
    sName = QString::fromStdWString(std::wstring(wnd_title));
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
        uiGameMillisecondsAvalable = uiGameMillisecondsAvalable + (ulGamingMilliseconds*2);
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
#ifndef WIN32
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
#else
        ShowWindow(hwnd, SW_FORCEMINIMIZE);
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

void CFreeDialog::on_toolButton_clicked()
{
    IdentifyGames();
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
