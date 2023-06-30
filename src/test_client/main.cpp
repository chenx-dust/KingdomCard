#include <spdlog/spdlog.h>
#include <zmq.hpp>
#include "basic_message.pb.h"

zmq::context_t context(1);

void tfunc (int tid) {
    spdlog::info("tid: {} 测试用客户端", tid);
    zmq::socket_t socket_req(context, ZMQ_REQ);
    socket_req.connect("tcp://localhost:13364");
    spdlog::info("tid: {} 连接成功", tid);

    BasicMessage req_m;
    req_m.set_type(CommandType::CONNECT_REQ);
    zmq::message_t req_z(req_m.ByteSizeLong());
    req_m.SerializeToArray(req_z.data(), req_z.size());
    socket_req.send(req_z, zmq::send_flags::none);
    spdlog::info("tid: {} 发送连接请求", tid);

    zmq::message_t rep_z;
    socket_req.recv(rep_z, zmq::recv_flags::none);
    BasicMessage rep_m;
    rep_m.ParseFromArray(rep_z.data(), rep_z.size());
    spdlog::info("tid: {} 收到回复，类型为{}", tid, CommandType_Name(rep_m.type()));
    ConnectResponse rep_r;
    rep_r.ParseFromString(rep_m.message());
    spdlog::info("tid: {} 玩家ID为{}，端口为{}", tid, rep_r.player_id(), rep_r.port());

    unsigned int id = rep_r.player_id();
    zmq::socket_t socket_pair(context, ZMQ_PAIR);
    socket_pair.connect("tcp://localhost:" + std::to_string(rep_r.port()));
    spdlog::info("tid: {} 连接成功", tid);
    BasicMessage ack_m;
    ack_m.set_type(CommandType::CONNECT_ACK);
    ack_m.set_message(std::to_string(id));
    zmq::message_t ack_z(ack_m.ByteSizeLong());
    ack_m.SerializeToArray(ack_z.data(), ack_z.size());
    socket_pair.send(ack_z, zmq::send_flags::none);
    spdlog::info("tid: {} 发送连接确认", tid);

    while (true) {
        zmq::message_t msg;
        socket_pair.recv(msg, zmq::recv_flags::none);
        BasicMessage m;
        m.ParseFromArray(msg.data(), msg.size());
        spdlog::info("tid: {} 收到消息，类型为 {}", tid, CommandType_Name(m.type()));
        if (m.type() == CommandType::GAME_START) {
            spdlog::info("tid: {} 游戏开始", tid);
            break;
        }
        if (m.type() == CommandType::CONNECT_ACK) {
            spdlog::info("tid: {} 检测到连接确认，玩家ID为{}", tid, m.player_id());
            ack_z = zmq::message_t(ack_m.ByteSizeLong());
            ack_m.SerializeToArray(ack_z.data(), ack_z.size());
            socket_pair.send(ack_z, zmq::send_flags::none);
        }
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
    {
        std::thread t(tfunc, i);
        threads.emplace_back(std::move(t));
    }
    for (auto &t : threads)
        t.join();
    return 0;
}