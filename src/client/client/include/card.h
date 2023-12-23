//
// Created by 10315 on 2023/12/20.
//

#ifndef KINGDOM_CARD_CARD_H
#define KINGDOM_CARD_CARD_H


#include <QPushButton>
//#include "../ui/ui_card.h"
#include "../../communicator/communicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Card : public QPushButton
{
Q_OBJECT

public:

    Card(QPushButton *parent = nullptr);
    Card(const Card&) = default;


    void SetImage();

private:
    void SetupUi();

public slots:

};


#endif //KINGDOM_CARD_CARD_H
