//
// Created by 10315 on 2023/12/20.
//

#include "../include/card.h"

void Card::SetImage() {

}

void Card::SetupUi() {
    setFixedSize(120, 180);
    setDisabled(true);
    setText(QString::fromStdString(UTILS::CardName[type]));
    connect(this, &QPushButton::clicked, this, &Card::CardClicked);
}

Card::Card(unsigned id, CARD_TYPE type, QPushButton *parent) : id(id), type(type) {

    SetupUi();
}

void Card::SetChecked(bool checked) {
    if (checked && !is_checked) {
        setText(QString::fromStdString(UTILS::CardName[type] + "\n（已选中）"));
    } else if (!checked && is_checked) {
        setText(QString::fromStdString(UTILS::CardName[type]));
    }
    is_checked = checked;
}

void Card::CardClicked() {
    SetChecked(!is_checked);
}
