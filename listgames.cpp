#include <QDebug>
#include <QSettings>
#include <QShortcut>

#include "listgames.h"
#include "ui_listgames.h"



#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include <unistd.h>
#include <X11/extensions/XTest.h>
extern "C" {
#include <xdo.h>
}



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
    // pressing DEL activates the slots only when list widget has focus
        QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listGames);
        connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
}

CListGames::~CListGames()
{
    delete ui;
}
void CListGames::deleteItem()
{
    delete ui->listGames->currentItem();
}
void CListGames::on_bnIdentifyGame_clicked()
{

    QSettings Settings;

    xdo_t* xdo;
    XID ulWindowId=NULL;

    xdo = xdo_new(NULL);
    int iErrorCode = xdo_select_window_with_click(xdo, &ulWindowId);

    unsigned char *name;
    int name_len;
    int name_type;

      xdo_get_window_name(xdo, ulWindowId, &name, &name_len, &name_type);
      std::string sName=std::string(reinterpret_cast<char*>(name));
      qDebug() << QString::fromStdString(sName) << " "<< name_len<< " "<< name_type << ulWindowId;
    ui->listGames->addItem(QString::fromStdString(sName));
//    xdo_close_window(xdo, ulWindowId);
//    xdo_kill_window(xdo, ulWindowId);
    // XFree(name); // apt-get install libfreetype6-dev maybe???
    xdo_free(xdo);

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
