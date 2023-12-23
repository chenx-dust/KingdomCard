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
    unsigned my_id;
    Ui::ClientWindow *ui;

    std::vector<std::unique_ptr<Card>> CardsInHand;
    std::vector<std::unique_ptr<Player>> PlayersInGame;

    void Log(const std::string &msg);
    void SetMyID(const BasicMessage &message);
    void SetGameStatus(const BasicMessage &message);
    void NewCard(const BasicMessage &message);
    void DiscardCard(const BasicMessage &message);

public slots:
    void SignalHandler(const BasicMessage &message);
};
#endif // MAINWINDOW_H
