//
// Created by 10315 on 2023/12/18.
//

#include "server.h"

ServerWindow::ServerWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::ServerWindow)
{
    ui->setupUi(this);
    _is_begin_connect = false;
    _is_game_start = false;

    connect(ui->CONNECT_REP, &QPushButton::clicked, this, &ServerWindow::ConnectRep);
    connect(ui->CONNECT_ACK, &QPushButton::clicked, this, &ServerWindow::ConnectACK);
    connect(ui->GAME_STATUS, &QPushButton::clicked, this, &ServerWindow::GameStatus);



    connect(&Communicator::communicator(), &Communicator::messageSend, this, &ServerWindow::BuildConnect);
    connect(ui->GAME_START, &QPushButton::clicked, this, &ServerWindow::GameStart);

}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::ConnectRep() {
    if (_is_begin_connect) {
        BasicMessage msg;
        msg.set_type(SIGNALS::CONNECT_REP);
        Communicator::communicator().sendSignal(msg);
    }
}

void ServerWindow::ConnectACK() {
    if (_is_begin_connect) {
        BasicMessage msg;
        msg.set_type(SIGNALS::CONNECT_ACK);
        Communicator::communicator().sendSignal(msg);
    }
}

void ServerWindow::BuildConnect(const BasicMessage &message) {
    if (_is_begin_connect) return;
    SIGNALS signal = message.type();
    if (signal == SIGNALS::CONNECT_REQ) _is_begin_connect = true;

    QMessageBox info;
    info.setText("connecting");
    info.exec();

}

void ServerWindow::GameStart() {
    if(_is_game_start) return;
    _is_game_start = true;
//    Communicator::communicator().sendSignal(SIGNALS::GAME_START);
    BasicMessage msg;
    msg.set_type(SIGNALS::GAME_START);
    Communicator::communicator().sendSignal(msg);
}

void ServerWindow::GameStatus() {
    if(!_is_game_start) return;
    UTILS::utils::PeopleCount = ui->Value->text().toInt();
//    Communicator::communicator().sendSignal(SIGNALS::GAME_STATUS);
    BasicMessage msg;
    msg.set_type(SIGNALS::GAME_STATUS);
    Communicator::communicator().sendSignal(msg);
}
