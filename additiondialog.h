#ifndef ADDITIONDIALOG_H
#define ADDITIONDIALOG_H

#include <QDialog>

namespace Ui {
class CAdditionDialog;
}

class CAdditionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CAdditionDialog(QWidget *parent = 0);
    ~CAdditionDialog();

private:
    Ui::CAdditionDialog *ui;
};

#endif // ADDITIONDIALOG_H
