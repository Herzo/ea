#ifndef FREEDIALOG_H
#define FREEDIALOG_H

#include <QSystemTrayIcon>
// #include <QDialog>
#include <QMainWindow>
#include <QNetworkReply>
#include "additiondialog.h"
#include "multiplicationdialog.h"

#include <QMap>
#include <QElapsedTimer>
class QMenu;

namespace Ui {
class CFreeDialog;
}

class CFreeDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit CFreeDialog(QWidget *parent = 0);
    ~CFreeDialog();
    void setVisible(bool visible);
#ifndef WIN32
    static QString GetVersion(){return QString("0.0.3L");};
#else
    static QString GetVersion(){return QString("0.0.3W");};
#endif
public slots:
    void on_ServerReply(QNetworkReply *pReply);
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

    void iconActivated(QSystemTrayIcon::ActivationReason reason);


    void on_CloseButton_clicked();

    void on_NextButton_clicked();
    void on_RemoveMark();

    void on_DifficultySlider_valueChanged(int value);

    void on_Update();
    void on_toolButton_clicked();

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
    void createTrayIcon();
    QAction *m_pMinimizeAction;
    QAction *m_pMaximizeAction;
    QAction *m_pRestoreAction;
    QAction *m_pQuitAction;
    QAction *m_pChangeSkin;
    QAction *m_pIdentifyGames;
    QAction *m_pIdentifyEducationals;
    QSystemTrayIcon *m_pTrayIcon;
    QMenu *m_pTrayIconMenu;
    QMenu *m_pMenu;
    QMap<QString, std::pair< QIcon, QString> > m_IconStore;
    void InitIconStore();

    void InitQuestion();
    QTimer *m_pTimer;
    void DisplayGameMinutes();
    uint GetQuestionsGameMilliseconds();
    void DisplayQuestionGameMinutes();
};


#endif // FREEDIALOG_H
