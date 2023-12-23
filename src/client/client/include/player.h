//
// Created by 10315 on 2023/12/22.
//

#ifndef KINGDOM_CARD_PLAYER_H
#define KINGDOM_CARD_PLAYER_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include <memory>

#include "../../communicator/communicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Player : public QVBoxLayout
{
Q_OBJECT

public:

    Player(QVBoxLayout *parent = nullptr);
    std::unique_ptr<QPushButton> PlayerCard;
    std::unique_ptr<QLabel> PlayerName;
    std::unique_ptr<QLabel> PlayerHP;
    std::unique_ptr<QLabel> PlayerIdentity;



    void SetImage();

private:
    void SetupUi();

public slots:

};

#endif //KINGDOM_CARD_PLAYER_H
