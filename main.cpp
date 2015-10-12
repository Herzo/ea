/*
 * File:   main.cpp
 * Author: adrian
 *
 * Created on 20 September 2015, 5:36 PM
 */


 #include <QApplication>
 #include <QMessageBox>

#include <QtGui>

#include "freedialog.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);
    QCoreApplication::setOrganizationName("Geobytes");
    QCoreApplication::setOrganizationDomain("geobytes.com");
    QCoreApplication::setApplicationName("Einstein's Agent");
    QApplication app(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QCoreApplication::applicationName(),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    CFreeDialog window;
    window.show();
    return app.exec();
}
