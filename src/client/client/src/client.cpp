#include <QMessageBox>
#include "../include/client.h"
#include "command.pb.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    ui->SelfCard->setFixedSize(160, 195);
    ui->SelfCard->setStyleSheet("border: 1px solid black");
    ui->PlayerBox->setGeometry(QRect(QPoint(110, 40), QSize(860, 270)));
    ui->GameTable->setReadOnly(true);
    ui->GameTable->appendPlainText(QString("欢迎游玩三国杀，游戏即将开始，请做好准备！\n"));

    connect(&Communicator::communicator(), &Communicator::messageRecv, this, &ClientWindow::SignalHandler);

    timer = new QTimer(this);
    // 30s to 100%
    time_interval = 30000 / (ui->progressBar->maximum() - ui->progressBar->minimum());
    timer->start(time_interval);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (ui->progressBar->value() < 100)
            ui->progressBar->setValue(ui->progressBar->value() + 1);
    });

    connect(ui->DiscardAction, &QPushButton::clicked, this, &ClientWindow::DiscardAction);
    connect(ui->PlayAction, &QPushButton::clicked, this, &ClientWindow::PlayAction);
}

ClientWindow::~ClientWindow()
{
    delete ui;
    delete timer;
}

void ClientWindow::SignalHandler(const BasicMessage &message) {
    SIGNALS signal = message.type();
    switch (signal) {
        case SIGNALS::CONNECT_REP:
            SetMyID(message);
            break;
        case SIGNALS::GAME_START:
            GameStart(message);
            break;
        case SIGNALS::GAME_STATUS:
            SetGameStatus(message);
            break;
        case SIGNALS::NEW_CARD:
            NewCard(message);
            break;
        case SIGNALS::DISCARD_CARD:
            DiscardCard(message);
            break;
        case SIGNALS::YOUR_TURN:
            YourTurn(message);
            break;
        case SIGNALS::NOTICE_CARD:
            NoticeCard(message);
            break;
        case SIGNALS::NOTICE_DYING:
            NoticeDying(message);
            break;
        case SIGNALS::NOTICE_DEAD:
            NoticeDead(message);
            break;
        case SIGNALS::GAME_OVER:
            GameOver(message);
            break;
        default:
            break;
    }
}

void ClientWindow::Log(const std::string &msg) {
    ui->GameTable->moveCursor(QTextCursor::End);
    ui->GameTable->insertPlainText(QString::fromStdString(msg));
}

void ClientWindow::GameStart(const BasicMessage &message) {
    this->show();
    class GameStart start;
    start.ParseFromString(message.message());
    lord_id = start.lordid();
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::GameStart: playeridentity: " << start.playeridentity() << " lordid: " << lord_id;
    ui->SelfIdentity->setText(QString("身份： ") + UTILS::PlayerIdentityName[start.playeridentity()].c_str());
}

void ClientWindow::SetMyID(const BasicMessage &message) {
    ConnectResponse rep;
    rep.ParseFromString(message.message());
    my_id = rep.player_id();
    ui->SelfName->setText(QString::fromStdString("玩家 ID：" + std::to_string(my_id)));
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::SetMyID: my_id: " << my_id;
}

void ClientWindow::SetGameStatus(const BasicMessage &message) {
    static bool inited = false;
    GameStatus status;
    status.ParseFromString(message.message());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::SetGameStatus: currentturnplayerid: " << status.currentturnplayerid()
                                 << " totalplayers: " << status.totalplayers();
    now_turn_id = status.currentturnplayerid();
    if (!inited) {
        for (int i = 0; i < status.totalplayers(); ++i) {
            if (status.players(i).id() == my_id) {
                connect(this, &ClientWindow::targetChange, this, [this](unsigned target_id) {
                    if (target_id == my_id) {
                        ui->SelfCard->setStyleSheet("border: 2px solid red");
                        ui->SelfCard->setText("（已选中）");
                    } else {
                        if (now_turn_id == my_id)
                            ui->SelfCard->setStyleSheet("border: 2px solid green");
                        else
                            ui->SelfCard->setStyleSheet("border: 1px solid black");
                        ui->SelfCard->setText("");
                    }
                });
                connect(ui->SelfCard, &QPushButton::clicked, this, [this]() {
                    if (this->target_id == my_id) {
                        this->target_id = -1;
                    } else {
                        this->target_id = my_id;
                    }
                    emit targetChange(target_id);
                });
                continue;
            }
            PlayersInGame.emplace_back(new Player(status.players(i).id()));
            ui->PlayerBox->addLayout(PlayersInGame.back().get());
            connect(this, &ClientWindow::targetChange, PlayersInGame.back().get(), &Player::TargetChangeNotice);
            connect(PlayersInGame.back().get()->PlayerCard.get(), &QPushButton::clicked, this,
                    [this, id = status.players(i).id()]() {
                        if (this->target_id == id) {
                            this->target_id = -1;
                        } else {
                            this->target_id = id;
                        }
                        emit targetChange(target_id);
                    });
        }
        inited = true;
    }
    Log("当前回合玩家 ID：" + std::to_string(status.currentturnplayerid())
        + (status.currentturnplayerid() == my_id ? "，是您的回合" : "") + "\n");
    if (status.currentturnplayerid() != my_id) {
        SetMyTurn(false);
        ui->progressBar->setValue(0);
        ui->progressBar->setDisabled(true);
        timer->start(time_interval);
    }
    for (int i = 0; i < status.totalplayers(); ++i) {
        if (status.players(i).id() == my_id) {
            ui->SelfHP->setText(QString::fromStdString("HP: " + std::to_string(status.players(i).hp())
                                                       + "/" + std::to_string(status.players(i).maxhp())));
            continue;
        }
        for (int j = 0; j < PlayersInGame.size(); ++j) {
            if (PlayersInGame[j]->GetID() == status.players(i).id()) {
                PlayersInGame[j]->UpdateStatus(status.players(i).hp(), status.players(i).maxhp(),
                                               status.players(i).cardcnt(),
                                               status.players(i).id() == lord_id,
                                               status.players(i).id() == status.currentturnplayerid());
                break;
            }
        }
    }
}
//void ClientWindow::StartGame(const BasicMessage &message) {
//    ui->CardBox->setGeometry(QRect(QPoint(210, 560), QSize(820, 200)));
//    for (int i = 0; i < CARD_ORIGIN_NUM; ++i) {
//        CardsInHand.emplace_back(new Card());
//        ui->CardBox->addWidget(CardsInHand[i].get());
//    }
//}

void ClientWindow::NewCard(const BasicMessage &message) {
    class NewCard new_card;
    new_card.ParseFromString(message.message());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::NewCard: newcards_size: " << new_card.newcards_size();
    for (int i = 0; i < new_card.newcards_size(); ++i) {
        CardsInHand.emplace_back(new Card(new_card.newcards(i).id(), new_card.newcards(i).type()));
        ui->CardBox->addWidget(CardsInHand.back().get());
    }
}

void ClientWindow::DiscardCard(const BasicMessage &message) {
    class DiscardCard discard_card;
    discard_card.ParseFromString(message.message());
    Log("您被迫弃掉了 " + std::to_string(discard_card.discardedcards_size()) + " 张牌，分别是：");
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::DiscardCard: discardedcards_size: " << discard_card.discardedcards_size();
    for (int i = 0; i < discard_card.discardedcards_size(); ++i) {
        for (int j = 0; j < CardsInHand.size(); ++j) {
            if (CardsInHand[j]->GetID() == discard_card.discardedcards(i).id()) {
                CardsInHand[j]->hide();
                CardsInHand.erase(CardsInHand.begin() + j);
                break;
            }
        }
        Log(UTILS::CardName[discard_card.discardedcards(i).type()]
            + (i != discard_card.discardedcards_size() - 1 ? "、" : "\n"));
    }
}

void ClientWindow::YourTurn(const BasicMessage &message) {
    SetMyTurn(true);
    class YourTurn your_turn;
    your_turn.ParseFromString(message.message());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::YourTurn: remainingtime: " << your_turn.remainingtime()
        << " turntype: " << your_turn.turntype();
    Log("您的回合，剩余时间：" + std::to_string(your_turn.remainingtime()) + "ms\n");
    turn_type = your_turn.turntype();

    for (auto & card : CardsInHand) {
        card->SetChecked(false);
        if (turn_type == TurnType_pb::ACTIVE)
            card->setDisabled(false);
        else
            if (TurnCardsAvailable[turn_type].find(card->GetType()) == TurnCardsAvailable[turn_type].end()) {
                card->setDisabled(true);
            } else {
                card->setDisabled(false);
            }
    }

    unsigned time = ui->progressBar->maximum() - your_turn.remainingtime() / time_interval;
    if (time > ui->progressBar->maximum()) {
        time = ui->progressBar->maximum();
    } else if (time < ui->progressBar->minimum()) {
        time = ui->progressBar->minimum();
    }
    ui->progressBar->setValue(time);
    ui->progressBar->setDisabled(false);
    timer->start(time_interval);
}

void ClientWindow::NoticeCard(const BasicMessage &message) {
    class NoticeCard notice_card;
    notice_card.ParseFromString(message.message());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::NoticCard: playerid: " << notice_card.playerid()
                                 << " targetplayerid: " << notice_card.targetplayerid()
                                 << " card.id: " << notice_card.card().id()
                                 << " card.type: " << notice_card.card().type();
    Log("玩家 " + std::to_string(notice_card.playerid()) + " 向目标玩家 "
        + std::to_string(notice_card.targetplayerid()) + " 打出了："
        + UTILS::CardName[notice_card.card().type()] + "\n");
}

void ClientWindow::DiscardAction() {
    ActionPass action;
    for (int i = 0; i < CardsInHand.size(); ++i) {
        if (CardsInHand[i]->GetChecked()) {
            Card_pb card;
            card.set_id(CardsInHand[i]->GetID());
            card.set_type(CardsInHand[i]->GetType());
            action.add_discardedcards()->CopyFrom(card);
        }
    }
    BasicMessage m;
    m.set_type(ACTION_PASS);
    m.set_message(action.SerializeAsString());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::DiscardAction: message: " << m.message();
    Communicator::communicator().sendSignal(m);
    Log("您弃掉了 " + std::to_string(action.discardedcards_size()) + " 张牌，分别是：");
    for (int i = 0; i < CardsInHand.size(); ++i) {
        if (CardsInHand[i]->GetChecked()) {
            Log(UTILS::CardName[CardsInHand[i]->GetType()] + " ");
            CardsInHand[i]->hide();
            CardsInHand.erase(CardsInHand.begin() + i);
            --i;
        } else {
            CardsInHand[i]->setDisabled(true);
        }
    }
    Log("\n");
}

void ClientWindow::SetMyTurn(bool turn) {
    my_turn = turn;
    ui->PlayAction->setDisabled(!turn);
    ui->DiscardAction->setDisabled(!turn);
    if (!turn) {
        for (auto & card : CardsInHand) {
            card->setDisabled(true);
        }
    }
    if (turn)
        ui->SelfCard->setStyleSheet("border: 2px solid green");
    else
        ui->SelfCard->setStyleSheet("border: 1px solid black");
}

void ClientWindow::NoticeDying(const BasicMessage &message) {
    class NoticeDying notice_dying;
    notice_dying.ParseFromString(message.message());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::NoticeDying: playerid: " << notice_dying.playerid();
    Log("玩家 " + std::to_string(notice_dying.playerid()) + " 即将死亡，等待救援\n");
    for (auto & player : PlayersInGame) {
        if (player->GetID() == notice_dying.playerid()) {
            player->SetDying(true);
            break;
        }
    }
}

void ClientWindow::NoticeDead(const BasicMessage &message) {
    class NoticeDead notice_dead;
    notice_dead.ParseFromString(message.message());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::NoticeDead: playerid: " << notice_dead.playerid();
    Log("玩家 " + std::to_string(notice_dead.playerid()) + " 已死亡\n");
    for (auto & player : PlayersInGame) {
        if (player->GetID() == notice_dead.playerid()) {
            player->SetDead();
            break;
        }
    }
}

void ClientWindow::GameOver(const BasicMessage &message) {
    class GameOver game_over;
    game_over.ParseFromString(message.message());
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::GameOver: victorycamp: " << game_over.victorycamp();
    Log("游戏结束，胜利者是阵营 " + UTILS::PlayerIdentityName[game_over.victorycamp()] + "\n");
    QMessageBox::information(this, "游戏结束", QString::fromStdString("游戏结束，胜利者是阵营 "
            + UTILS::PlayerIdentityName[game_over.victorycamp()]));
    this->close();
}
