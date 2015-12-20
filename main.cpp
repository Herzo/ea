/*
 * File:   main.cpp
 * Author: adrian
 *
 * Created on 20 September 2015, 5:36 PM
 */


 #include <QApplication>
 #include <QMessageBox>
#include <QSharedMemory>
#include <QtGui>
// #include "mainwindow.h"



#include "freedialog.h"
// #include "optionsdialog.h"
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);
    QCoreApplication::setOrganizationName("Geobytes");
    QCoreApplication::setOrganizationDomain("geobytes.com");
    QCoreApplication::setApplicationName("Einstein's Agent");
    QApplication app(argc, argv);

    QSharedMemory mem("einstein");
    if(!mem.create(sizeof( quint64 )))
    {
        // It appears that we may be already running in another instance.
        // Attempt to activate window on the instance of ourself that is alreay running
        // Signal the other instance, and wait 15 sceonds for it to reply.
        // If it replies then we terminate, if not then we reset the signal and continue.
        mem.attach();
        void* pData=mem.data();
        if(pData!=NULL)
        {
            quint64 &uiData = *(quint64*)(pData);
            uiData=1;
            uint uiSleepFor=2000; // two seconds
        #if defined Q_OS_WIN
            Sleep(uiSleepFor);
        #elif defined Q_OS_MACX
            struct timespec ts = { static_cast<__darwin_time_t>(uiSleepFor / 1000), (uiSleepFor % 1000) * 1000 * 1000 };
            nanosleep(&ts, NULL);
        #endif
          // If the other instance of ourself got the signal then it will have set it back to zero
            if(uiData==0)
            {
                exit(0);
            }else
            {
                uiData=0;
            }
        }
    }
#if not defined Q_OS_IOS
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QCoreApplication::applicationName(),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
#endif

    QApplication::setQuitOnLastWindowClosed(false);
    // CMainWindow window;
    CFreeDialog window;
    // COptionsDialog window;
    window.show();
    return app.exec();
}
