/* 
 * File:   window.cpp
 * Author: adrian
 * 
 * Created on 20 September 2015, 6:35 PM
 */
 #include <QtGui>

 #include <QAction>
 #include <QCheckBox>
 #include <QComboBox>
 #include <QGroupBox>
 #include <QLabel>
 #include <QLineEdit>
 #include <QMenu>
 #include <QPushButton>
 #include <QSpinBox>
 #include <QTextEdit>
 #include <QVBoxLayout>
 #include <QMessageBox>

 #include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QMetaEnum>
#include <QDebug>

#include "optionsdialog.h"
#include "o1khan.h"
#include "changeuserdialog.h"

const char MY_CLIENT_ID[] = "uCj7JaPgnqVPaPDD";
const char MY_CLIENT_SECRET[] = "FcwDL3xeY826yGUY";

COptionsDialog::COptionsDialog()
{
    createIconGroupBox();
    createMessageGroupBox();

    m_pIconLabel->setMinimumWidth(m_pDurationLabel->sizeHint().width());

    createActions();
    createTrayIcon();

    connect(m_pShowMessageButton, SIGNAL(clicked()), this, SLOT(showMessage()));
//    connect(m_pShowIconCheckBox, SIGNAL(toggled(bool)),
//            m_pTrayIcon, SLOT(showMessage()));
    connect(m_pShowIconCheckBox, SIGNAL(toggled(bool)),
            m_pTrayIcon, SLOT(setVisible(bool)));
    connect(m_pIconComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setIcon(int)));
    connect(m_pTrayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    m_pOAuthKhan = new O1Khan(this);
    m_pOAuthKhan->setClientId(MY_CLIENT_ID);
    m_pOAuthKhan->setClientSecret(MY_CLIENT_SECRET);
    connect(m_pOAuthKhan, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(m_pOAuthKhan, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
    connect(m_pOAuthKhan, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(m_pOAuthKhan, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(m_pOAuthKhan, SIGNAL(closeBrowser()), this, SLOT(onCloseBrowser()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pIconGroupBox);
    mainLayout->addWidget(m_pMessageGroupBox);
    setLayout(mainLayout);

    m_pIconComboBox->setCurrentIndex(1);
    m_pTrayIcon->show();

    setWindowTitle(tr("Einstein's Agent"));
    resize(400, 300);
}

void COptionsDialog::setVisible(bool visible)
{
    m_pMinimizeAction->setEnabled(visible);
    m_pMaximizeAction->setEnabled(!isMaximized());
    m_pRestoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void COptionsDialog::closeEvent(QCloseEvent *event)
{
    if (m_pTrayIcon->isVisible()) {
        QMessageBox::information(this, tr("Einstein's Agent"),
                                 tr("Einstein's Agent will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}

void COptionsDialog::setIcon(int index)
{
    QIcon icon = m_pIconComboBox->itemIcon(index);
    m_pTrayIcon->setIcon(icon);
    setWindowIcon(icon);

    m_pTrayIcon->setToolTip(m_pIconComboBox->itemText(index));
}

void COptionsDialog::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        m_pIconComboBox->setCurrentIndex((m_pIconComboBox->currentIndex() + 1)
                                      % m_pIconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}

void COptionsDialog::showMessage()
{
    CChangeUserDialog dl;
    dl.exec();

//    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
//            m_pTypeComboBox->itemData(m_pTypeComboBox->currentIndex()).toInt());
//    m_pTrayIcon->showMessage(m_pTitleEdit->text(), m_pBodyEdit->toPlainText(), icon,
//                          m_pDurationSpinBox->value() * 1000);
}

void COptionsDialog::messageClicked()
{
    QMessageBox::information(0, tr("Einstein's Agent"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void COptionsDialog::createIconGroupBox()
{
    m_pIconGroupBox = new QGroupBox(tr("Active User"));

    m_pIconLabel = new QLabel("Icon:");

    m_pIconComboBox = new QComboBox;
    m_pIconComboBox->addItem(QIcon(":/images/running.svg"), tr("Running"));
    m_pIconComboBox->addItem(QIcon(":/images/play43.svg"), tr("Heart"));
    m_pIconComboBox->addItem(QIcon(":/images/trash.svg"), tr("Trash"));

    m_pShowIconCheckBox = new QCheckBox(tr("Show icon"));
    m_pShowIconCheckBox->setChecked(true);

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(m_pIconLabel);
    iconLayout->addWidget(m_pIconComboBox);
    iconLayout->addStretch();
    iconLayout->addWidget(m_pShowIconCheckBox);
    m_pIconGroupBox->setLayout(iconLayout);
}

void COptionsDialog::createMessageGroupBox()
{
    m_pMessageGroupBox = new QGroupBox(tr("Balloon Message"));

    m_pTypeLabel = new QLabel(tr("Type:"));

    m_pTypeComboBox = new QComboBox;
    m_pTypeComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
    m_pTypeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxInformation), tr("Information"),
            QSystemTrayIcon::Information);
    m_pTypeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxWarning), tr("Warning"),
            QSystemTrayIcon::Warning);
    m_pTypeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxCritical), tr("Critical"),
            QSystemTrayIcon::Critical);
    m_pTypeComboBox->setCurrentIndex(1);

    m_pDurationLabel = new QLabel(tr("Duration:"));

    m_pDurationSpinBox = new QSpinBox;
    m_pDurationSpinBox->setRange(0, 10000);
    m_pDurationSpinBox->setSuffix(" s");
    m_pDurationSpinBox->setValue(1000);
    m_pDurationSpinBox->setSingleStep(1000);

    m_pDurationWarningLabel = new QLabel(tr("(some systems might ignore this "
                                         "hint)"));
    m_pDurationWarningLabel->setIndent(10);

    m_pTitleLabel = new QLabel(tr("Title:"));

    m_pTitleEdit = new QLineEdit(tr("Cannot connect to network"));

    m_pBodyLabel = new QLabel(tr("Body:"));

    m_pBodyEdit = new QTextEdit;
    m_pBodyEdit->setPlainText(tr("Don't believe me. Honestly, I don't have a "
                              "clue.\nClick this balloon for details."));

    m_pShowMessageButton = new QPushButton(tr("Show Message"));
    m_pShowMessageButton->setDefault(true);

    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(m_pTypeLabel, 0, 0);
    messageLayout->addWidget(m_pTypeComboBox, 0, 1, 1, 2);
    messageLayout->addWidget(m_pDurationLabel, 1, 0);
    messageLayout->addWidget(m_pDurationSpinBox, 1, 1);
    messageLayout->addWidget(m_pDurationWarningLabel, 1, 2, 1, 3);
    messageLayout->addWidget(m_pTitleLabel, 2, 0);
    messageLayout->addWidget(m_pTitleEdit, 2, 1, 1, 4);
    messageLayout->addWidget(m_pBodyLabel, 4, 0);
    messageLayout->addWidget(m_pBodyEdit, 4, 1, 2, 4);
    messageLayout->addWidget(m_pShowMessageButton, 3, 2);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    m_pMessageGroupBox->setLayout(messageLayout);
}

void COptionsDialog::createActions()
{
    m_pMinimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(m_pMinimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    m_pMaximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(m_pMaximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    m_pRestoreAction = new QAction(tr("&Restore"), this);
    connect(m_pRestoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_pQuitAction = new QAction(tr("&Quit"), this);
    connect(m_pQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void COptionsDialog::createTrayIcon()
{
    m_pTrayIconMenu = new QMenu(this);
    m_pTrayIconMenu->addAction(m_pMinimizeAction);
    m_pTrayIconMenu->addAction(m_pMaximizeAction);
    m_pTrayIconMenu->addAction(m_pRestoreAction);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pQuitAction);

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
}
void COptionsDialog::onOpenBrowser(const QUrl &url) {
    // Open a web browser or a web view with the given URL.
    // The user will interact with this browser window to
    // enter login name, password, and authorize your application
    // to access the Twitter account
    QDesktopServices::openUrl(url);

}
