#include <QMessageBox>
#include "../include/client.h"
#include "command.pb.h"

void ClientWindow::PlayAction() {
    ActionPlay action;
    action.set_targetplayerid(target_id);
    unsigned total_cards = 0, card_iter = 0;
    for (int i = 0; i < CardsInHand.size(); ++i) {
        if (CardsInHand[i]->GetChecked()) {
            ++total_cards;
            card_iter = i;
        }
    }
    if (total_cards != 1) {
        QMessageBox warning;
        warning.setText("您只能打出一张牌！");
        warning.exec();
        return;
    }
    Card_pb last_card;
    last_card.set_id(CardsInHand[card_iter]->GetID());
    last_card.set_type(CardsInHand[card_iter]->GetType());
    if (TurnCardsAvailable[turn_type].find(last_card.type()) == TurnCardsAvailable[turn_type].end()) {
        QMessageBox warning;
        warning.setText("您不能打出这张牌！");
        warning.exec();
        return;
    }
    switch (CardsInHand[card_iter]->GetType()) {
        // 无指定目标
        case CARD_TYPE::ARCHERY_VOLLEY:
        case CARD_TYPE::BARBARIAN:
        case CARD_TYPE::SLEIGHT_OF_HAND:
        case CARD_TYPE::HARVEST_FEAST:
        case CARD_TYPE::PEACH_GARDEN_OATH:
            emit targetChange(-1);
            break;
            // 有指定目标
        case CARD_TYPE::SLASH:
        case CARD_TYPE::DISMANTLE:
        case CARD_TYPE::STEAL:
        case CARD_TYPE::DUEL:
            if (target_id == -1) {
                QMessageBox warning;
                warning.setText("您没有选中任何目标！");
                warning.exec();
                return;
            }
            if (target_id == my_id) {
                QMessageBox warning;
                warning.setText("您不能对自己使用！");
                warning.exec();
                return;
            }
            break;
        case CARD_TYPE::PEACH:
            if (target_id == -1) {
                QMessageBox warning;
                warning.setText("您没有选中任何目标！");
                warning.exec();
                return;
            }
            for (auto &player : PlayersInGame) {
                if (player->GetID() == target_id) {
                    if (player->GetHP() == player->GetMP()) {
                        QMessageBox warning;
                        warning.setText("您不能对血量已满的玩家使用桃！");
                        warning.exec();
                        return;
                    }
                    break;
                }
            }
            break;
        default:
            QMessageBox warning;
            warning.setText("您不能打出这张牌！");
            warning.exec();
            return;
    }
    action.mutable_card()->CopyFrom(last_card);
    action.set_targetplayerid(target_id);
    CardsInHand[card_iter]->hide();
    CardsInHand.erase(CardsInHand.begin() + card_iter);
    BasicMessage m;
    m.set_type(ACTION_PLAY);
    m.set_message(action.SerializeAsString());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::PlayAction: message: " << m.message();
    Communicator::communicator().sendSignal(m);
    Log("您打出了 " + UTILS::CardName[last_card.type()] + " ");
    if (target_id != (unsigned)-1)
        Log("目标玩家 ID：" + std::to_string(target_id));
    Log("\n");
    timer->stop();
}
