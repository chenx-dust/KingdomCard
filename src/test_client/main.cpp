#include <spdlog/spdlog.h>
#include <zmq.hpp>
#include "basic_message.pb.h"

int main() {
    spdlog::info("测试用客户端");
    zmq::context_t context(1);
    zmq::socket_t socket_req(context, ZMQ_REQ);
    socket_req.connect("tcp://localhost:13364");
    spdlog::info("连接成功");

    BasicMessage req_m;
    req_m.set_type(CommandType::CONNECT_REQ);
    zmq::message_t req_z(req_m.ByteSizeLong());
    req_m.SerializeToArray(req_z.data(), req_z.size());
    socket_req.send(req_z, zmq::send_flags::none);
    spdlog::info("发送连接请求");

    zmq::message_t rep_z;
    socket_req.recv(rep_z, zmq::recv_flags::none);
    BasicMessage rep_m;
    rep_m.ParseFromArray(rep_z.data(), rep_z.size());
    spdlog::info("收到回复，类型为{}", CommandType_Name(rep_m.type()));
    ConnectResponse rep_r;
    rep_r.ParseFromString(rep_m.message());
    spdlog::info("玩家ID为{}，端口为{}", rep_r.player_id(), rep_r.port());

    unsigned int id = rep_r.player_id();
    zmq::socket_t socket_pair(context, ZMQ_PAIR);
    socket_pair.connect("tcp://localhost:" + std::to_string(rep_r.port()));
    spdlog::info("连接成功");
    BasicMessage ack_m;
    ack_m.set_type(CommandType::CONNECT_ACK);
    ack_m.set_message(std::to_string(id));
    zmq::message_t ack_z(ack_m.ByteSizeLong());
    ack_m.SerializeToArray(ack_z.data(), ack_z.size());
    socket_pair.send(ack_z, zmq::send_flags::none);
    spdlog::info("发送连接确认");

    while (true) {
        zmq::message_t msg;
        socket_pair.recv(msg, zmq::recv_flags::none);
        BasicMessage m;
        m.ParseFromArray(msg.data(), msg.size());
        spdlog::info("收到消息，类型为{}", CommandType_Name(m.type()));
        if (m.type() == CommandType::GAME_START) {
            spdlog::info("游戏开始");
            break;
        }
        if (m.type() == CommandType::CONNECT_ACK) {
            spdlog::info("检测到连接确认，玩家ID为{}", m.player_id());
            ack_z = zmq::message_t(ack_m.ByteSizeLong());
            ack_m.SerializeToArray(ack_z.data(), ack_z.size());
            socket_pair.send(ack_z, zmq::send_flags::none);
        }
    }

    return 0;
}