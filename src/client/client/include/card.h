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

    Card(unsigned id, CARD_TYPE type, QPushButton *parent = nullptr);
    unsigned GetID() const { return id; }
    CARD_TYPE GetType() const { return type; }
    void SetChecked(bool);
    bool GetChecked() const { return is_checked;}
    void SetImage();

private:
    unsigned id;
    CARD_TYPE type;
    bool is_checked;

    void SetupUi();

public slots:
    void CardClicked();

};


#endif //KINGDOM_CARD_CARD_H
