#ifndef LISTEDUCATIONALS_H
#define LISTEDUCATIONALS_H

#include <QDialog>

namespace Ui {
class CListEducationals;
}

class CListEducationals : public QDialog
{
    Q_OBJECT

public:
    explicit CListEducationals(QWidget *parent = 0);
    ~CListEducationals();
   // bool eventFilter(QObject *watched, QEvent *event);

protected slots:
    void InitSkin();

private slots:

    void on_buttonBox_accepted();
    void deleteItem();


    void on_bnAddEducationalManually_clicked();

    void on_toolButtonAddEducational_clicked();

private:
    Ui::CListEducationals *ui;
    void DeleteItem();
};

#endif // LISTEDUCATIONALS_H
