//
// Created by 10315 on 2023/12/18.
//

#include "communicator.h"
#include <QDebug>

void Communicator::init(QString address, unsigned port) {
    communicator().thread = new Spinner(&communicator(), address, port);
    communicator().thread->start();
}

Communicator::~Communicator() {
    this->thread->quit();
    this->thread->wait();
    delete this->thread;
}

void Communicator::sendSignal(const BasicMessage &message) {
    emit this->messageSend(message);
}

Communicator::Spinner::Spinner(Communicator* comm, QString address, unsigned port, QObject *parent) : QThread(parent) {
    this->address = address;
    this->port = port;
    this->communicator = comm;
}

void Communicator::Spinner::run() {
    communicator->socket_init.connect("tcp://" + address.toStdString() + ":" + std::to_string(port));
    BasicMessage msg;
    msg.set_type(SIGNALS::CONNECT_REQ);
    zmq::message_t connect_req(msg.ByteSizeLong());
    msg.SerializeToArray(connect_req.data(), connect_req.size());
    communicator->socket_init.send(connect_req, zmq::send_flags::none);
    QDebug(QtMsgType::QtInfoMsg) << "Communicator::spin: send CONNECT_REQ";

    // 等待连接确认
    zmq::message_t connect_rep;
    while (!communicator->socket_init.recv(connect_rep, zmq::recv_flags::none).has_value());
    BasicMessage reply_msg;
    reply_msg.ParseFromArray(connect_rep.data(), connect_rep.size());
    QDebug(QtMsgType::QtDebugMsg) << "Communicator::spin: recv CONNECT_ACK";
    ConnectResponse connect_ack;
    connect_ack.ParseFromString(reply_msg.message());
    QDebug(QtMsgType::QtInfoMsg) << "Communicator::spin: player_id: " << connect_ack.player_id() << " port: " << connect_ack.port();
    // 连接服务器
    communicator->socket.connect("tcp://" + address.toStdString() + ":" + std::to_string(connect_ack.port()));

    // 发送连接确认
    BasicMessage connect_ack_m;
    connect_ack_m.set_type(SIGNALS::CONNECT_ACK);
    connect_ack_m.set_message(std::to_string(connect_ack.player_id()));
    zmq::message_t connect_ack_z(connect_ack_m.ByteSizeLong());
    connect_ack_m.SerializeToArray(connect_ack_z.data(), connect_ack_z.size());
    communicator->socket.send(connect_ack_z, zmq::send_flags::none);

    communicator->player_id = connect_ack.player_id();
    while (true) {
        zmq::message_t request;
        auto rtn = communicator->socket.recv(request, zmq::recv_flags::none);
        if (!rtn.has_value()) {
            QDebug(QtMsgType::QtWarningMsg) << "Communicator::spin: recv failed";
            continue;
        }
        BasicMessage message;
        message.ParseFromArray(request.data(), request.size());

        if (message.type() == CONNECT_ACK) {
            QDebug(QtMsgType::QtInfoMsg) << "Communicator::spin: recv CONNECT_ACK";
            BasicMessage ack_m;
            ack_m.set_type(SIGNALS::CONNECT_ACK);
            ack_m.set_message(std::to_string(communicator->player_id));
            zmq::message_t ack_z(ack_m.ByteSizeLong());
            ack_m.SerializeToArray(ack_z.data(), ack_z.size());
            communicator->socket.send(ack_z, zmq::send_flags::none);
        } else
            emit communicator->messageRecv(message);
    }
}

void Communicator::sendSignal_impl(const BasicMessage &message) {
    zmq::message_t request(message.ByteSizeLong());
    message.SerializeToArray(request.data(), request.size());
    socket.send(request, zmq::send_flags::none);
}

Communicator *Communicator::instance = nullptr;
