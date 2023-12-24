//
// Created by 10315 on 2023/12/22.
//

#include "../include/player.h"

void Player::SetImage() {

}

void Player::SetupUi() {
    PlayerCard = std::make_unique<QPushButton>(new QPushButton);
    PlayerCard->setFixedSize(160, 195);
    PlayerCard->setStyleSheet("border: 1px solid black");
    PlayerName = std::make_unique<QLabel>(new QLabel);
    PlayerName->setText("玩家 ID： " + QString::number(id));
    PlayerHP = std::make_unique<QLabel>(new QLabel);
    PlayerHP->setText("HP: ");
    PlayerIdentity = std::make_unique<QLabel>(new QLabel);
    PlayerIdentity->setText("玩家身份： ");
    PlayerCardCnt = std::make_unique<QLabel>(new QLabel);
    PlayerCardCnt->setText("手牌数： ");
    this->addWidget(PlayerCard.get());
    this->addWidget(PlayerName.get());
    this->addWidget(PlayerHP.get());
    this->addWidget(PlayerCardCnt.get());
    this->addWidget(PlayerIdentity.get());
}

Player::Player(unsigned id, QVBoxLayout *parent) : id(id) {
    SetupUi();
}

void Player::UpdateStatus(int hp, int mp, int card_cnt, bool is_lord, bool is_now_turn) {
    this->hp = hp;
    this->mp = mp;
    PlayerHP->setText("HP: " + QString::number(hp) + "/" + QString::number(mp));
    PlayerCardCnt->setText("手牌数： " + QString::number(card_cnt));
    PlayerIdentity->setText(is_lord ? "身份： 主公" : "身份： 未知");
    if (is_dead)
        return;
    if (is_now_turn) {
        PlayerCard->setStyleSheet("border: 2px solid green");
    } else {
        PlayerCard->setStyleSheet("border: 1px solid black");
    }
    if (is_target) {
        PlayerCard->setStyleSheet("border: 2px solid red");
        PlayerCard->setText("（已选中）");
    } else {
        PlayerCard->setText("");
    }
    now_turn = is_now_turn;
}

void Player::TargetChangeNotice(unsigned target_id) {
    if (target_id == id) {
        PlayerCard->setStyleSheet("border: 2px solid red");
        PlayerCard->setText("（已选中）");
        is_target = true;
    } else {
        if (now_turn) {
            PlayerCard->setStyleSheet("border: 2px solid green");
        } else {
            PlayerCard->setStyleSheet("border: 1px solid black");
        }
        PlayerCard->setText("");
        is_target = false;
    }
}

void Player::SetDead() {
    is_dead = true;
    PlayerCard->setStyleSheet("border: 2px solid black");
    PlayerCard->setText("（已死亡）");
    PlayerCard->setEnabled(false);
}

void Player::SetDying(bool dying) {
    if (dying) {
        PlayerCard->setStyleSheet("border: 2px solid yellow");
    } else {
        if (now_turn) {
            PlayerCard->setStyleSheet("border: 2px solid green");
        } else {
            PlayerCard->setStyleSheet("border: 1px solid black");
        }
    }
}
