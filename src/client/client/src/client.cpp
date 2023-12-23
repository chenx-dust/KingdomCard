#include "../include/client.h"
#include "command.pb.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    ui->PlayerBox->setGeometry(QRect(QPoint(110, 40), QSize(860, 270)));
    ui->GameTable->setReadOnly(true);
    ui->GameTable->appendPlainText(QString("欢迎游玩三国杀，游戏即将开始，请做好准备！\n"));

    connect(&Communicator::communicator(), &Communicator::messageRecv, this, &ClientWindow::SignalHandler);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::SignalHandler(const BasicMessage &message) {
    SIGNALS signal = message.type();
    switch (signal) {
        case SIGNALS::CONNECT_REP:
            SetMyID(message);
            break;
        case SIGNALS::GAME_START:
            this->show();
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
        default:
            break;
    }
}

void ClientWindow::Log(const std::string &msg) {
    ui->GameTable->appendPlainText(QString::fromStdString(msg));
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
    QDebug(QtMsgType::QtInfoMsg) << "ClientWindow::SetGameStatus: totalplayers: " << status.totalplayers()
                                    << " lordid: " << status.lordid();
    if (!inited) {
        Log("游戏开始！本局主公是：" + std::to_string(status.lordid())
            + "，当前回合玩家：" + std::to_string(status.currentturnplayerid()) + "\n");
        for (int i = 0; i < status.totalplayers(); ++i) {
            if (status.players(i).id() == my_id) {
                ui->SelfIdentity->setText(QString("身份： ") + (status.players(i).id() == status.lordid() ? "主公" : "未知"));
                continue;
            }
            PlayersInGame.emplace_back(new Player(status.players(i).id()));
            ui->PlayerBox->addLayout(PlayersInGame.back().get());
        }
        inited = true;
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
                                               status.players(i).id() == status.lordid());
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
    for (int i = 0; i < new_card.newcards_size(); ++i) {
        CardsInHand.emplace_back(new Card(new_card.newcards(i).id(), new_card.newcards(i).type()));
        ui->CardBox->addWidget(CardsInHand.back().get());
    }
}

void ClientWindow::DiscardCard(const BasicMessage &message) {
    Card card;
    card.ParseFromString(message.message());
    for (int i = 0; i < CardsInHand.size(); ++i) {
        if (CardsInHand[i]->GetID() == card.id()) {
            CardsInHand[i]->hide();
            CardsInHand.erase(CardsInHand.begin() + i);
            break;
        }
    }
}

