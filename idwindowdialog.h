#ifndef IDWINDOWDIALOG_H
#define IDWINDOWDIALOG_H
#ifdef WIN32
#include <windows.h>
#endif
#include <QDialog>

namespace Ui {
class CIdWindowDialog;
}

class CIdWindowDialog : public QDialog
{
    Q_OBJECT

public:
    // QString m_sWindowName;

#ifdef WIN32
    POINT m_ScreenPoint;
#endif
    explicit CIdWindowDialog(QWidget *parent = 0);
    ~CIdWindowDialog();
protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_CIdWindowDialog_accepted();

private:
    Ui::CIdWindowDialog *ui;
};

#endif // IDWINDOWDIALOG_H
