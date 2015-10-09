#ifndef CHANGEUSERDIALOG_H
#define CHANGEUSERDIALOG_H

#include <QDialog>

namespace Ui {
class CChangeUserDialog;
}

class CChangeUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CChangeUserDialog(QWidget *parent = 0);
    ~CChangeUserDialog();

private:
    Ui::CChangeUserDialog *ui;
};

#endif // CHANGEUSERDIALOG_H
