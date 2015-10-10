#ifndef FREEDIALOG_H
#define FREEDIALOG_H

#include <QDialog>

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
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pushButton_clicked();
    void setIcon(int index);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::CFreeDialog *ui;
    void createActions();
    void createTrayIcon();
    QAction *m_pMinimizeAction;
    QAction *m_pMaximizeAction;
    QAction *m_pRestoreAction;
    QAction *m_pQuitAction;

    QSystemTrayIcon *m_pTrayIcon;
    QMenu *m_pTrayIconMenu;
    QMap<QSting, pair< icon, QString> > m_IconStore;
    InitIconStore();
};

#endif // FREEDIALOG_H
