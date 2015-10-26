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
    if(!mem.create(1))
    {
       // Attempt to activate window on the instance of ourself that is alreay running
       exit(0);
    }
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QCoreApplication::applicationName(),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    CFreeDialog window;
    // COptionsDialog window;
    window.show();
    return app.exec();
}
