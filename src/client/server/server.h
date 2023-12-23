//
// Created by 10315 on 2023/12/18.
//

#ifndef KINGDOM_CARD_SERVER_H
#define KINGDOM_CARD_SERVER_H

#include "./ui_serverwindow.h"
#include "../communicator/communicator.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE

class ServerWindow : public QMainWindow
{
Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();



private slots:
    void ConnectRep();
    void ConnectACK();
    void GameStatus();


    void BuildConnect(SIGNALS signal);
    void StopConnect(SIGNALS signal);
    void GameStart();

private:
    Ui::ServerWindow *ui;
    bool _is_begin_connect;
    bool _is_game_start;


};

#endif //KINGDOM_CARD_SERVER_H
