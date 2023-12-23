//
// Created by 10315 on 2023/12/19.
//

#include "../include/start.h"
#include <QMessageBox>


StartWindow::StartWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    ui->Password->setEchoMode(QLineEdit::Password);
    _button_toggled = false;

    connect(ui->GameStart, &QPushButton::clicked, this, &StartWindow::GameStart);
    connect(&Communicator::communicator(), &Communicator::messageSent, this, &StartWindow::ShutDown);


}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::GameStart() {
    if(!_button_toggled) {
        account = ui->Account->text();
        password = ui->Password->text();
        if (!account.isEmpty() and !password.isEmpty()) {
            ui->Account->setReadOnly(true);
            ui->Password->setReadOnly(true);

            Communicator::communicator().sendSignal(SIGNALS::CONNECT_REQ);
            ui->GameStart->setText("取消连接");
            ui->status_text->setText("等待服务器进行连接");
        } else {
            QMessageBox warning;
            warning.setText("账号密码不能为空");
            warning.exec();
        }
        _button_toggled = true;
    }else{
        Communicator::communicator().sendSignal(SIGNALS::CONNECT_INTERRUPTED);
        ui->Account->setReadOnly(false);
        ui->Password->setReadOnly(false);
        ui->GameStart->setText("开始游戏");
        ui->status_text->setText("");
        _button_toggled = false;
    }
}

void StartWindow::ShutDown(SIGNALS signal) {
    if (signal == SIGNALS::CONNECT_ACK and _button_toggled){
        this->hide();
    }
}

