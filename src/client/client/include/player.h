//
// Created by 10315 on 2023/12/22.
//

#ifndef KINGDOM_CARD_PLAYER_H
#define KINGDOM_CARD_PLAYER_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include <memory>

#include "../../communicator/communicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Player : public QVBoxLayout
{
Q_OBJECT

public:

    Player(unsigned id, QVBoxLayout *parent = nullptr);
    std::unique_ptr<QPushButton> PlayerCard;
    std::unique_ptr<QLabel> PlayerName;
    std::unique_ptr<QLabel> PlayerHP;
    std::unique_ptr<QLabel> PlayerCardCnt;
    std::unique_ptr<QLabel> PlayerIdentity;

    void SetImage();
    unsigned GetID() const { return id; }
    int GetHP() const { return hp; }
    int GetMP() const { return mp; }
    bool IsDead() const { return is_dead; }
    void SetDead();
    void UpdateStatus(int hp, int mp, int card_cnt, bool is_lord, bool is_now_turn);
    void SetDying(bool);

private:
    bool now_turn = false;
    unsigned id;
    int hp, mp;
    bool is_dead = false;
    bool is_target = false;
    void SetupUi();

public slots:
    void TargetChangeNotice(unsigned target_id);
};

#endif //KINGDOM_CARD_PLAYER_H
