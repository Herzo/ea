#ifndef GETWINDOWTITLEDIALOG_H
#define GETWINDOWTITLEDIALOG_H

#include <QDialog>

namespace Ui {
class CGetWindowTitleDialog;
}

class CGetWindowTitleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CGetWindowTitleDialog(QWidget *parent = 0);
    ~CGetWindowTitleDialog();
    QString m_sTitle;
    void SetHeading(QString sHeading);
private slots:
    void on_buttonBox_accepted();

private:
    void InitSkin();
    Ui::CGetWindowTitleDialog *ui;
};

#endif // GETWINDOWTITLEDIALOG_H
