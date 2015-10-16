#include <QDebug>
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
}

CListGames::~CListGames()
{
    delete ui;
}

void CListGames::on_bnIdentifyGame_clicked()
{
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
    // XFree(name);
    xdo_free(xdo);
}
