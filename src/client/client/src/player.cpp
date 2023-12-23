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
    PlayerName->setText("玩家名： ");
    PlayerHP = std::make_unique<QLabel>(new QLabel);
    PlayerHP->setText("HP: ");
    PlayerIdentity = std::make_unique<QLabel>(new QLabel);
    PlayerIdentity->setText("玩家身份： ");
    this->addWidget(PlayerCard.get());
    this->addWidget(PlayerName.get());
    this->addWidget(PlayerHP.get());
    this->addWidget(PlayerIdentity.get());
}

Player::Player(QVBoxLayout *parent) {
    SetupUi();
}