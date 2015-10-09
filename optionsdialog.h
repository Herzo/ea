/* 
 * File:   window.h
 * Author: adrian
 *
 * Created on 20 September 2015, 6:35 PM
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QSystemTrayIcon>
#include <QDialog>

#include "o1khan.h"
#include "o1requestor.h"

class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;

class COptionsDialog : public QDialog
{
    Q_OBJECT

public:
    COptionsDialog();

    void setVisible(bool visible);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void setIcon(int index);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();
    void onLinkedChanged() {
        // Linking (login) state has changed.
        // Use o1->linked() to get the actual state
        qDebug() << "Link changed!";
    }

    void onLinkingFailed() {
        // Login has failed
    }

    void onLinkingSucceeded() {
        // Login has succeeded
    }

    void onOpenBrowser(const QUrl &url);

    void onCloseBrowser() {
        // Close the browser window opened in openBrowser()
    }
private:
    void createIconGroupBox();
    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();

    QGroupBox *m_pIconGroupBox;
    QLabel *m_pIconLabel;
    QComboBox *m_pIconComboBox;
    QCheckBox *m_pShowIconCheckBox;

    QGroupBox *m_pMessageGroupBox;
    QLabel *m_pTypeLabel;
    QLabel *m_pDurationLabel;
    QLabel *m_pDurationWarningLabel;
    QLabel *m_pTitleLabel;
    QLabel *m_pBodyLabel;
    QComboBox *m_pTypeComboBox;
    QSpinBox *m_pDurationSpinBox;
    QLineEdit *m_pTitleEdit;
    QTextEdit *m_pBodyEdit;
    QPushButton *m_pShowMessageButton;

    QAction *m_pMinimizeAction;
    QAction *m_pMaximizeAction;
    QAction *m_pRestoreAction;
    QAction *m_pQuitAction;

    QSystemTrayIcon *m_pTrayIcon;
    QMenu *m_pTrayIconMenu;
    O1Khan *m_pOAuthKhan;
};

#endif
