//
// Created by 10315 on 2023/12/18.
//

#ifndef KINGDOM_CARD_COMMUNICATOR_H
#define KINGDOM_CARD_COMMUNICATOR_H

#include "commands.h"
#include "../utils/utils.h"
#include <QObject>
#include <QThread>
#include <zmq.hpp>
#include "basic_message.pb.h"

class Communicator : public QObject {
    Q_OBJECT

public:
    static Communicator& communicator() {
        if (instance == nullptr)
            instance = new Communicator();
        return *instance;
    }

    static void init(QString address, unsigned port);

signals:
    void messageSend(const BasicMessage message);
    void messageRecv(BasicMessage message);
public slots:
    void sendSignal(const BasicMessage &message);

private:
    static Communicator *instance;
    QThread *thread;
    zmq::context_t context {1};
    zmq::socket_t socket_init {context, ZMQ_REQ};
    zmq::socket_t socket {context, ZMQ_PAIR};
    unsigned player_id = -1;

    void sendSignal_impl(const BasicMessage &message);

    class Spinner : public QThread {
        QString address;
        unsigned port;
        Communicator *communicator;
    public:
        Spinner(Communicator *comm, QString address, unsigned port, QObject *parent = nullptr);
        void run() override;
    };
    ~Communicator();
};


#endif //KINGDOM_CARD_COMMUNICATOR_H
