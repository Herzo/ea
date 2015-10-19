#ifndef LISTGAMES_H
#define LISTGAMES_H

#include <QDialog>

namespace Ui {
class CListGames;
}

class CListGames : public QDialog
{
    Q_OBJECT

public:
    explicit CListGames(QWidget *parent = 0);
    ~CListGames();
   // bool eventFilter(QObject *watched, QEvent *event);

protected slots:
    void InitSkin();

private slots:
    void on_bnIdentifyGame_clicked();

    void on_buttonBox_accepted();
    void deleteItem();

private:
    Ui::CListGames *ui;
    void DeleteItem();
};

#endif // LISTGAMES_H
