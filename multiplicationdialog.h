#ifndef MULTIPLICATIONDIALOG_H
#define MULTIPLICATIONDIALOG_H

#include <QDialog>

namespace Ui {
class CMultiplicationDialog;
}

class CMultiplicationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CMultiplicationDialog(QWidget *parent = 0);
    ~CMultiplicationDialog();

private:
    Ui::CMultiplicationDialog *ui;
};

#endif // MULTIPLICATIONDIALOG_H
