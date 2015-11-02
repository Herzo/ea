#ifndef SPLASHDIALOG_H
#define SPLASHDIALOG_H

#include <QDialog>

namespace Ui {
class CSplashDialog;
}

class CSplashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSplashDialog(QWidget *parent = 0);
    ~CSplashDialog();

private slots:
    void on_checkBoxShowSplash_clicked(bool checked);

    void on_pushButton_clicked();

private:
    Ui::CSplashDialog *ui;
};

#endif // SPLASHDIALOG_H
