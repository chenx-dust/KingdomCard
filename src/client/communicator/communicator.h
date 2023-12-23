//
// Created by 10315 on 2023/12/18.
//

#ifndef KINGDOM_CARD_COMMUNICATOR_H
#define KINGDOM_CARD_COMMUNICATOR_H

#include "commands.h"
#include "../utils/utils.h"
#include <QObject>


class Communicator : public QObject {
    Q_OBJECT

public:
    static Communicator& communicator(){
        static Communicator communicator;
        return communicator;
    }


signals:
    void messageSent(SIGNALS signal);
public slots:
    void sendSignal(SIGNALS signal);

private:
    Communicator(){}
};


#endif //KINGDOM_CARD_COMMUNICATOR_H
