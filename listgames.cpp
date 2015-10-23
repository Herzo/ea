#include <QDebug>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QWidget>
#include "listgames.h"
#include "ui_listgames.h"


#ifndef WIN32
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include <unistd.h>
#include <X11/extensions/XTest.h>
#include <X11/cursorfont.h>
// #include <xdo.h>
extern "C" {
#include <xdo.h>
}
#else
#include <windows.h>
#include <cassert>
#endif
#include <idwindowdialog.h>


CListGames::CListGames(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CListGames)
{
    ui->setupUi(this);
    QSettings Settings;
    ui->listGames->clear();
    int size = Settings.beginReadArray("games");
    for (int i = 0; i < size; ++i)
    {
        Settings.setArrayIndex(i);
        QString sGameWindowTitle = Settings.value("windowtitle").toString();
        ui->listGames->addItem(sGameWindowTitle);
    }
    Settings.endArray();
    // ui->listGames->installEventFilter(this);
    // pressing DEL activates the slots only when list widget has focus
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listGames);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
    InitSkin();
}

CListGames::~CListGames()
{
    delete ui;
}
void CListGames::InitSkin()
{
    QSettings Settings;
    QPixmap img(Settings.value("skinlistgames",":/images/gameselectionherzo.png").toString());
    ui->skin->setPixmap(img);
}
/*
bool CListGames::eventFilter(QObject *watched, QEvent *event)
{
     if (event->type() == 2)  // QEvent::KeyPress==2 namespace clash with xdo
     {
         QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

         if (keyEvent->key()==Qt::Key_Delete)
         {
            QTimer::singleShot(50, this, SLOT(deleteItem()));
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
*/
void CListGames::deleteItem()
{
    delete ui->listGames->currentItem();
}
void CListGames::on_bnIdentifyGame_clicked()
{
    QSettings Settings;
    QString sName;

#ifndef WIN32

    // x_window_cursor_set
    // x_window_focus
    // x_window_show
    xdo_t* xdo;
    XID ulWindowId=0;
    xdo = xdo_new(NULL);
    if(xdo==NULL)
    {
        qDebug() << "xdo could not get a new display";
        return;
    }
    xdo->close_display_when_freed=true;
    xdo_select_window_with_click(xdo, &ulWindowId);

    unsigned char *name;
    int name_len;
    int name_type;

    xdo_get_window_name(xdo, ulWindowId, &name, &name_len, &name_type);
    sName = QString::fromStdString(std::string(reinterpret_cast<char*>(name)));
    XFree(name); // need to add -lX11 to LIBS in project
    xdo_free(xdo);

#else
    // setOverrideCursor(QCursor(QPixmap("wireless.jpg")));
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));


    // load a copy of the cross cursor
    HCURSOR hcCross = LoadCursor(0, IDC_CROSS);
    // assert(hcCross);
    HCURSOR hcCrossCopy = CopyCursor(hcCross);
    // assert(hcCrossCopy);

    // load a copy of the arrow cursor (so we can restore it)
    HCURSOR hcArrow = LoadCursor(0, IDC_ARROW);
    // assert(hcArrow);
    HCURSOR hcArrowCopy = CopyCursor(hcArrow);
    // assert(hcArrowCopy);

    // make the system arrow cursor the Cross cursor
    // NOTE: IDC_ARROW == MAKEINTRESOURCE(OCR_NORMAL)
    // cout << "Changing normal cursor to Cross cursor" << endl;
    BOOL ret = SetSystemCursor(hcCrossCopy, 32512);
    // assert(ret);

    CIdWindowDialog dl;
    dl.exec();
    wchar_t wnd_title[512];
    HWND hwnd=WindowFromPoint(dl.m_ScreenPoint); // get handle of currently active window
    GetWindowText(hwnd,wnd_title,sizeof(wnd_title));
    sName=QString::fromStdWString(std::wstring(wnd_title));
    activateWindow();

    // restore the arrow cursor
    ret = SetSystemCursor(hcArrowCopy, 32512);
    assert(ret);

    DestroyCursor(hcCross);
    DestroyCursor(hcArrow);


    QApplication::restoreOverrideCursor();
#endif
    // sName="solitaire";
    sName.trimmed();
    if(sName!="")
    {
        ui->listGames->addItem(sName);
    }
    QApplication::setActiveWindow(this);
}

void CListGames::on_buttonBox_accepted()
{
    QSettings settings;
    settings.beginWriteArray("games");
    for (int i = 0; i < ui->listGames->count(); ++i)
    {
        ui->listGames->setCurrentRow(i);
        settings.setArrayIndex(i);
        settings.setValue("windowtitle", ui->listGames->currentItem()->text());
    }
    settings.endArray();
}

