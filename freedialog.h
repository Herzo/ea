#ifndef FREEDIALOG_H
#define FREEDIALOG_H

#include <QSystemTrayIcon>
// #include <QDialog>
#include <QMainWindow>

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
    static QString CFreeDialog::GetVersion(){return QString("0.0.2")};
public slots:
    void on_ServerReply(QNetworkReply *pReply);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

protected slots:
    void ChangeSkin();
    void IdentifyGames();
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

    void on_FetchGameFingerprints();
private:
    void setIcon(QString sMode);
    QElapsedTimer m_GamingTimer;
    // unsigned long m_ulLastGamingTime;
    unsigned long m_ulWindowId;
    double m_dQuestionDifficultyMultiplier;
    int m_iAnswer;
    Ui::CFreeDialog *ui;
    void createActions();
    void createTrayIcon();
    void createAppMenu();
    QAction *m_pMinimizeAction;
    QAction *m_pMaximizeAction;
    QAction *m_pRestoreAction;
    QAction *m_pQuitAction;
    QAction *m_pChangeSkin;
    QAction *m_pIdentifyGames;

    QSystemTrayIcon *m_pTrayIcon;
    QMenu *m_pTrayIconMenu;
    QMenu *m_pMenu;
    QMap<QString, std::pair< QIcon, QString> > m_IconStore;
    void InitIconStore();

    void InitQuestion();
    QTimer *m_pTimer;
    void DisplayGameMinutes();
    uint GetQuestionsGameMilliseconds();
    void InitSkin();
    void DisplayQuestionGameMinutes();
};


#endif // FREEDIALOG_H
