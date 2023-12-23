//
// Created by 10315 on 2023/12/22.
//

#include "../include/player.h"

void Player::SetImage() {

}

void Player::SetupUi() {
    PlayerCard = std::make_unique<QPushButton>(new QPushButton);
    PlayerCard->setFixedSize(160, 195);
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

void Player::UpdateStatus(int hp, int mp, int card_cnt, bool is_lord) {
    PlayerHP->setText("HP: " + QString::number(hp) + "/" + QString::number(mp));
    PlayerCardCnt->setText("手牌数： " + QString::number(card_cnt));
    PlayerIdentity->setText(is_lord ? "身份： 主公" : "身份： 未知");
}
