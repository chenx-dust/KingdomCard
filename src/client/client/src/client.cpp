#include "../include/client.h"


ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    ui->PlayerBox->setGeometry(QRect(QPoint(110, 40), QSize(860, 270)));
    ui->GameTable->setReadOnly(true);
    ui->GameTable->appendPlainText(QString("欢迎游玩三国杀，游戏即将开始，请做好准备！\n"));

    connect(&Communicator::communicator(), &Communicator::messageRecv, this, &ClientWindow::StartGame);
    connect(&Communicator::communicator(), &Communicator::messageRecv, this, &ClientWindow::ShowWindow);
    connect(&Communicator::communicator(), &Communicator::messageRecv, this, &ClientWindow::SetGameStatus);

}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::StartGame(const BasicMessage &message) {
    SIGNALS signal = message.type();
    if (signal == SIGNALS::GAME_START){
        ui->CardBox->setGeometry(QRect(QPoint(210, 560), QSize(820, 200)));
        for (int i = 0; i < CARD_ORIGIN_NUM; ++i) {
            CardsInHand.emplace_back(new Card());
            ui->CardBox->addWidget(CardsInHand[i].get());
        }
    }
}

void ClientWindow::ShowWindow(const BasicMessage &message) {
    SIGNALS signal = message.type();
    if (signal == SIGNALS::GAME_START) {
        this->show();
    }
}

void ClientWindow::SetGameStatus(const BasicMessage &message) {
    SIGNALS signal = message.type();
    if (signal == SIGNALS::GAME_STATUS){
        for (int i = 0; i < UTILS::utils::PeopleCount-1; ++i) {
            PlayersInGame.emplace_back(new Player());
            ui->PlayerBox->addLayout(PlayersInGame[i].get());
        }
    }
}

