//
// Created by 10315 on 2023/12/19.
//

#include "../include/start.h"
#include <QMessageBox>


StartWindow::StartWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::StartWindow) {
    ui->setupUi(this);

    connect(ui->GameStart, &QPushButton::clicked, this, &StartWindow::GameStart);
    connect(ui->ServerAddr, &QLineEdit::returnPressed, this, &StartWindow::GameStart);
    connect(ui->ServerPort, &QLineEdit::returnPressed, this, &StartWindow::GameStart);
    connect(&Communicator::communicator(), &Communicator::messageRecv, this, &StartWindow::ShutDown);
}

StartWindow::~StartWindow() {
    delete ui;
}

void StartWindow::GameStart() {
    try {
        address = ui->ServerAddr->text();
        port = ui->ServerPort->text().toInt();
        if (address.isEmpty() or port == 0)
            throw std::runtime_error("服务器地址端口不能为空");

        Communicator::init(address, port);
    }
    catch (std::exception &e) {
        QMessageBox warning;
        warning.setText(e.what());
        warning.exec();
        return;
    }
    ui->ServerAddr->setReadOnly(true);
    ui->ServerPort->setReadOnly(true);
    ui->GameStart->setDisabled(true);
    ui->GameStart->setText("正在连接");
    ui->status_text->setText("等待服务器开始游戏");
}

void StartWindow::ShutDown(const BasicMessage &message) {
    auto signal = message.type();
    if (signal == SIGNALS::GAME_START) {
        this->hide();
    } else if (signal == SIGNALS::KICK) {
        QMessageBox warning;
        warning.setText("您已被踢出游戏");
        warning.exec();
        this->close();
    }
}

