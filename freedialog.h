#ifndef FREEDIALOG_H
#define FREEDIALOG_H

#include <QSystemTrayIcon>
#include <QDialog>
#include <QMap>
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

    bool eventFilter(QObject *watched, QEvent *event);
protected:
    void closeEvent(QCloseEvent *event);

protected slots:
    void ChangeSkin();
    void on_CloseEye();
private slots:
   // void on_pushButton_clicked();
    void setIcon(QString sMode);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    // void on_buttonBox_accepted();

    void on_CloseButton_clicked();

    void on_NextButton_clicked();
    void on_RemoveMark();

    void on_DifficultySlider_valueChanged(int value);

    void on_Update();
private:
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

    QSystemTrayIcon *m_pTrayIcon;
    QMenu *m_pTrayIconMenu;
    QMap<QString, std::pair< QIcon, QString> > m_IconStore;
    void InitIconStore();

    void InitQuestion();
    int InverseOperator(int iOperator);
    int Operation(int iOperand1, int iOperator, int iOperand2);
    QTimer *m_pTimer;
    void DisplayGameMinutes();
    double GetQuestionsGameMinutes();
    void InitSkin();
};


#endif // FREEDIALOG_H
