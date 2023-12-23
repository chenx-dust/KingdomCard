#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>
#include <memory>

#include "../ui/ui_clientwindow.h"
#include "../../communicator/communicator.h"
#include "card.h"
#include "player.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private:
    Ui::ClientWindow *ui;

    std::vector<std::unique_ptr<Card>> CardsInHand;
    std::vector<std::unique_ptr<Player>> PlayersInGame;

    const int CARD_ORIGIN_NUM = 4;

public slots:
    void StartGame(SIGNALS signal);
    void ShowWindow(SIGNALS signal);
    void SetGameStatus(SIGNALS signal);
};
#endif // MAINWINDOW_H
