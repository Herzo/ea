#ifndef FREEDIALOG_H
#define FREEDIALOG_H

// #if not defined Q_OS_IOS && not defined Q_OS_ANDROID
#include <QSystemTrayIcon>
// #endif
#include <QDialog>
//#include <QMainWindow>
#include <QNetworkReply>
#include "additiondialog.h"
#include "multiplicationdialog.h"

#include <QMap>
#include <QElapsedTimer>
#if defined Q_OS_WIN  // Implement genWin32ShellExecute() especially for UAC
    #include "qt_windows.h"
    #include "qwindowdefs_win.h"
    #include <shellapi.h>
#endif
class QMenu;

namespace Ui {
class CFreeDialog;
}

class CFreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CFreeDialog(QWidget *parent = 0);
    ~CFreeDialog();
    void setVisible(bool visible);
    QString GetUuId();
#if defined Q_OS_ANDROID
    static QString GetVersion(){return QString("0.0.6A");};
#elif defined Q_OS_LINUX
    static QString GetVersion(){return QString("0.0.6L");};
#elif defined Q_OS_WIN
    static QString GetVersion(){return QString("0.0.6W");};
#elif defined Q_OS_MACX
    static QString GetVersion(){return QString("0.0.6M");};
#elif defined Q_OS_IOS
    static QString GetVersion(){return QString("0.0.6I");};
#endif
public slots:
    void on_ServerReply(QNetworkReply *pReply);
    void quit();
protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

protected slots:
    void ChangeSkin();
    void IdentifyGames();
    void IdentifyEducationals();
    void on_CloseEye();
    void on_ControlGames();

private slots:

//#if not defined Q_OS_IOS && not defined Q_OS_ANDROID
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
//#endif
    void on_CloseButton_clicked();
    void on_NextButton_clicked();
    void on_RemoveMark();
    void on_DifficultySlider_valueChanged(int value);
    void on_Update();
    void on_actionID_Games_triggered();
    void on_actionChange_Skin_triggered();
    void on_actionAbout_Einstein_s_Agent_triggered();
    void on_FetchEducationalFingerPrints();
    void on_FetchGameFingerPrints();
    void on_InitSkin();
    void on_pushButtonGameMinOK_clicked();
    void on_actionManage_Educationals_triggered();
    void on_checkBoxMultiplication_clicked(bool checked);
    void on_checkBoxAddition_clicked(bool checked);
    void on_ShowSplash();
    void on_actionHelp_triggered();
    void on_btnChangeSkin_clicked();

private:
    CAdditionDialog m_AdditionDialog;
    CMultiplicationDialog m_MultiplicationDialog;
    void setIcon(QString sMode);
    QElapsedTimer m_GamingTimer;
    // unsigned long m_ulLastGamingTime;
    unsigned long m_ulWindowId;
    double m_dQuestionDifficultyMultiplier;
    int m_iAnswer;
    Ui::CFreeDialog *ui;
    void createActions();
#if not defined Q_OS_IOS && not defined Q_OS_ANDROID
    void createTrayIcon();
#endif
    QAction *m_pMinimizeAction;
    QAction *m_pMaximizeAction;
    QAction *m_pRestoreAction;
    QAction *m_pQuitAction;
    QAction *m_pChangeSkin;
    QAction *m_pIdentifyGames;
    QAction *m_pIdentifyEducationals;
    QSystemTrayIcon *m_pTrayIcon;
#if not defined Q_OS_IOS && not defined Q_OS_ANDROID
    QMenu *m_pTrayIconMenu;
#endif
    QMenu *m_pMenu;
    QMap<QString, std::pair< QIcon, QString> > m_IconStore;
    void InitIconStore();

    void InitQuestion();
    QTimer *m_pTimer;
    void DisplayGameMinutes();
    uint GetQuestionsGameMilliseconds();
    void DisplayQuestionGameMinutes();
    QPoint m_pointLastMousePos;
    int m_iMouseIdleCounter;
#if defined Q_OS_WIN  // Implement genWin32ShellExecute() especially for UAC
    int genWin32ShellExecute(QString AppFullPath,
                         QString Verb,
                         QString Params,
                         bool ShowAppWindow,
                         bool WaitToFinish);
#endif
};


#endif // FREEDIALOG_H
