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

private slots:
    void on_pushButton_clicked();

private:
    Ui::CFreeDialog *ui;
};

#endif // FREEDIALOG_H
