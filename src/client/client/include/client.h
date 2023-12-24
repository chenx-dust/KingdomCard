#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>
#include <memory>
#include <QTimer>

#include "../ui/ui_clientwindow.h"
#include "../../communicator/communicator.h"
#include "card.h"
#include "player.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const std::vector<std::set<CardType_pb>> TurnCardsAvailable = {
        {CardType_pb::SLASH, CardType_pb::DISMANTLE, CardType_pb::STEAL, CardType_pb::ARCHERY_VOLLEY,
         CardType_pb::BARBARIAN, CardType_pb::SLEIGHT_OF_HAND, CardType_pb::HARVEST_FEAST, CardType_pb::PEACH,
         CardType_pb::PEACH_GARDEN_OATH, CardType_pb::DUEL},
        {CardType_pb::UNRELENTING},
        {CardType_pb::UNRELENTING, CardType_pb::SLASH},
        {CardType_pb::UNRELENTING, CardType_pb::DODGE},
        {CardType_pb::SLASH},
        {CardType_pb::DODGE},
        {CardType_pb::PEACH, CardType_pb::PEACH_GARDEN_OATH}
};

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private:
    unsigned my_id, lord_id;
    Ui::ClientWindow *ui;
    QTimer *timer;
    unsigned time_interval;
    bool my_turn = false;
    int target_id = -1;

    TurnType_pb turn_type = TurnType_pb::ACTIVE;

    std::vector<std::unique_ptr<Card>> CardsInHand;
    std::vector<std::unique_ptr<Player>> PlayersInGame;

    void Log(const std::string &msg);
    void GameStart(const BasicMessage &message);
    void SetMyID(const BasicMessage &message);
    void SetGameStatus(const BasicMessage &message);
    void NewCard(const BasicMessage &message);
    void DiscardCard(const BasicMessage &message);
    void YourTurn(const BasicMessage &message);
    void NoticeCard(const BasicMessage &message);
    void SetMyTurn(bool turn);

public slots:
    void SignalHandler(const BasicMessage &message);
    void DiscardAction();
    void PlayAction();
signals:
    void targetChange(unsigned target_id);
};
#endif // MAINWINDOW_H
