#include <QDebug>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QWidget>
#include <set>
#include "listeducationals.h"
#include "ui_listeducationals.h"


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


CListEducationals::CListEducationals(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CListEducationals)
{
    ui->setupUi(this);
    QSettings Settings;
    ui->listEducationals->clear();
    int size = Settings.beginReadArray("educationals");
    for (int i = 0; i < size; ++i)
    {
        Settings.setArrayIndex(i);
        QString sGameWindowTitle = Settings.value("windowtitle").toString();
        ui->listEducationals->addItem(sGameWindowTitle);
    }
    Settings.endArray();
    // ui->listEducationals->installEventFilter(this);
    // pressing DEL activates the slots only when list widget has focus
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listEducationals);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
    InitSkin();
}

CListEducationals::~CListEducationals()
{
    delete ui;
}
void CListEducationals::InitSkin()
{
    QSettings Settings;
    QPixmap img(Settings.value("skinlisteducationals",":/images/gameselectionherzo.png").toString());
    ui->skin->setPixmap(img);
}

void CListEducationals::deleteItem()
{
    delete ui->listEducationals->currentItem();
}
void CListEducationals::on_bnIdentifyEducationals_clicked()
{
    QSettings Settings;
    QString sName;

#ifndef WIN32

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
        ui->listEducationals->addItem(sName);
    }
    QApplication::setActiveWindow(this);
}

void CListEducationals::on_buttonBox_accepted()
{
    // Write the list in to a set first to remove the dups
    std::set<QString> Items;
    for (int i = 0; i < ui->listEducationals->count(); ++i)
    {
        QString sItem=ui->listEducationals->currentItem()->text();
        Items.insert(sItem.trimmed());
    }

    QSettings settings;
    settings.beginWriteArray("educationals");
    int iElement=0;
    std::set<QString>::const_iterator it;
    while (it!=Items.end())
    {
        ui->listEducationals->setCurrentRow(iElement);
        settings.setArrayIndex(iElement);
        settings.setValue("windowtitle", (*it));
        iElement++;
        it++;
    }
    settings.endArray();
}

