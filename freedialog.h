#ifndef FREEDIALOG_H
#define FREEDIALOG_H

#include <QSystemTrayIcon>
#include <QDialog>
#include <QMap>
#include <QElapsedTimer>
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
    int InverseOperator(int iOperator);
    int Operation(int iOperand1, int iOperator, int iOperand2);
    QTimer *m_pTimer;
    void DisplayGameMinutes();
    uint GetQuestionsGameMilliseconds();
    void InitSkin();
    void DisplayQuestionGameMinutes();
};


#endif // FREEDIALOG_H
