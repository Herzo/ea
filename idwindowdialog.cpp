#ifdef WIN32
#include <windows.h>
#endif
#include <QTimer>
#include <QEvent>
#include "idwindowdialog.h"
#include "ui_idwindowdialog.h"

CIdWindowDialog::CIdWindowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CIdWindowDialog)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);
    // setVisible(false);
    // showMaximized();
    // ShowWindow(this->winId(),SW_SHOWMAXIMIZED);
    grabMouse();
}

CIdWindowDialog::~CIdWindowDialog()
{
    releaseMouse();
    qApp->removeEventFilter(this);
    delete ui;
}

void CIdWindowDialog::on_CIdWindowDialog_accepted()
{
    close();
}
bool CIdWindowDialog::eventFilter(QObject *watched, QEvent *event)
{
    // qDebug() << "Event Type: "<< event->type();
     if (event->type() == QEvent::MouseMove)  // QEvent::KeyPress==6 namespace clash with xdo
     {
#ifdef WIN32
         // Keep our invisible dialog under the mouse pointer
         // QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));

         /*         POINT		screenpoint;
         GetCursorPos (&screenpoint);
         QPoint Point;
         Point.setX(screenpoint.x);
         Point.setY(screenpoint.y);
*/

        // move(QCursor::pos());
#endif
     }else if (event->type() == QEvent::ApplicationDeactivated)  // QEvent::KeyPress==6 namespace clash with xdo
     {
#ifdef WIN32
         GetCursorPos (&m_ScreenPoint);
         /*
         wchar_t wnd_title[512];
         setVisible(false);
         HWND hwnd=WindowFromPoint(screenpoint); // get handle of currently active window
         GetWindowText(hwnd,wnd_title,sizeof(wnd_title));
         m_sWindowName=QString::fromStdWString(std::wstring(wnd_title));
         */
#endif
         // emit closeEvent();
         QTimer::singleShot(0, this, SLOT(on_CIdWindowDialog_accepted()));
         return true;

     }
     return QObject::eventFilter(watched, event);

}
