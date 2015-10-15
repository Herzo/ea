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

private slots:
    void on_bnIdentifyGame_clicked();

private:
    Ui::CListGames *ui;
};

#endif // LISTGAMES_H
