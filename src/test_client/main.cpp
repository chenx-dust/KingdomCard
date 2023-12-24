#include <spdlog/spdlog.h>
#include <zmq.hpp>
#include "basic_message.pb.h"
#include "command.pb.h"

#define GET_ID(card) (card >> 16)
#define GET_TYPE(card) (card & 0xffff)
#define GET_CARD(id, type) (((uint16_t)(id) << 16) | (uint16_t)(type))

std::string const CardName[] = { "杀", "闪", "桃",
                                 "过河拆桥", "顺手牵羊", "决斗", "万箭齐发", "南蛮入侵", "无中生有", "五谷丰登", "桃园结义",
                                 "无懈可击" };
std::string const PlayerIdentityName[] = {
        "主公",
        "忠臣",
        "反贼",
        "内奸"
};

class Client {
    zmq::socket_t socket_pair;
    std::thread thread;
    size_t id;
    size_t tid;
    std::vector<uint64_t> cards;
public:
    Client(zmq::context_t &context, size_t tid) : tid(tid) {
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
        spdlog::info("tid: {} 收到回复, 类型为{}", tid, CommandType_Name(rep_m.type()));
        ConnectResponse rep_r;
        rep_r.ParseFromString(rep_m.message());
        spdlog::info("tid: {} 玩家ID为{}, 端口为{}", tid, rep_r.player_id(), rep_r.port());

        id = rep_r.player_id();
        socket_pair = zmq::socket_t(context, ZMQ_PAIR);
        socket_pair.connect("tcp://localhost:" + std::to_string(rep_r.port()));
        spdlog::info("tid: {} 连接成功", tid);
        BasicMessage ack_m;
        ack_m.set_type(CommandType::CONNECT_ACK);
        ack_m.set_message(std::to_string(id));
        zmq::message_t ack_z(ack_m.ByteSizeLong());
        ack_m.SerializeToArray(ack_z.data(), ack_z.size());
        socket_pair.send(ack_z, zmq::send_flags::none);
        spdlog::info("tid: {} 发送连接确认", tid);
        thread = std::thread(&Client::spin, this);
    }

    ~Client() {
        thread.join();
    }

    [[noreturn]] void spin() {
        // 等待游戏开始
        BasicMessage ack_m;
        ack_m.set_type(CommandType::CONNECT_ACK);
        ack_m.set_message(std::to_string(id));
        zmq::message_t ack_z(ack_m.ByteSizeLong());
        ack_m.SerializeToArray(ack_z.data(), ack_z.size());
        socket_pair.send(ack_z, zmq::send_flags::none);

        while (true) {
            zmq::message_t msg;
            socket_pair.recv(msg, zmq::recv_flags::none);
            BasicMessage m;
            m.ParseFromArray(msg.data(), msg.size());
            spdlog::debug("tid: {} 收到消息, 类型为 {}", tid, CommandType_Name(m.type()));
            if (m.type() == CommandType::GAME_START) {
                GameStart start;
                start.ParseFromString(m.message());
                spdlog::info("tid: {} 游戏开始, 身份为 {}, 主公 id: {}", tid,
                             PlayerIdentityName[start.playeridentity()], start.lordid());
                break;
            }
            else if (m.type() == CommandType::CONNECT_ACK) {
                spdlog::info("tid: {} 检测到连接确认, 玩家 id: {}", tid, m.player_id());
                ack_z = zmq::message_t(ack_m.ByteSizeLong());
                ack_m.SerializeToArray(ack_z.data(), ack_z.size());
                socket_pair.send(ack_z, zmq::send_flags::none);
            }
        }

        // 游戏开始后
        while (true) {
            zmq::message_t msg;
            socket_pair.recv(msg, zmq::recv_flags::none);
            BasicMessage m;
            m.ParseFromArray(msg.data(), msg.size());
            spdlog::debug("tid: {} 收到消息, 类型为 {}", tid, CommandType_Name(m.type()));
//            switch (m.type()) {
            if(m.type() == GAME_STATUS) {
                spdlog::debug("tid: {} 接收到游戏状态", tid);
                GameStatus status;
                status.ParseFromString(m.message());
                if (id == status.currentturnplayerid()) {
                    spdlog::info("tid: {} 是当前回合玩家", tid);
                    spdlog::info("tid: {} 总玩家数: {}, 当前轮 id: {}",
                                  tid, status.totalplayers(), status.currentturnplayerid());
                    for (int i = 0; i < status.players_size(); ++i) {
                        spdlog::info("tid: {} 玩家 id: {}, hp: {}, mp: {}, 玩家手牌数: {}",
                                      tid, status.players(i).id(), status.players(i).hp(),
                                      status.players(i).maxhp(), status.players(i).cardcnt());
                    }
                }
            } else if (m.type() == NOTICE_CARD) {
                spdlog::debug("tid: {} 接收到出牌信息", tid);
                NoticeCard notice;
                notice.ParseFromString(m.message());
                if (id == notice.playerid()) {
                    spdlog::info("tid: {} 玩家 id: {} 出牌, 目标 id: {}",
                                 tid, notice.playerid(), notice.targetplayerid());
                    spdlog::info("tid: {} 牌面 id: {}, type: {}",
                                 tid, notice.card().id(), CardName[notice.card().type()]);
                }
            } else if (m.type() == NOTICE_DYING) {
                spdlog::debug("tid: {} 接收到濒死信息", tid);
                NoticeDying notice;
                notice.ParseFromString(m.message());
                if (id == notice.playerid()) {
                    spdlog::info("tid: {} 玩家 id: {} 濒死", tid, notice.playerid());
                }
            } else if (m.type() == NOTICE_DEAD) {
                spdlog::debug("tid: {} 接收到死亡信息", tid);
                NoticeDead notice;
                notice.ParseFromString(m.message());
                if (id == notice.playerid()) {
                    spdlog::info("tid: {} 玩家 id: {} 死亡", tid, notice.playerid());
                }
            } else if (m.type() == GAME_OVER) {
                spdlog::debug("tid: {} 接收到游戏结束信息", tid);
                GameOver notice;
                notice.ParseFromString(m.message());
                spdlog::info("tid: {} 游戏结束, 胜利阵营: {}", tid, PlayerIdentityName[notice.victorycamp()]);
            } else if (m.type() == YOUR_TURN) {
                YourTurn notice;
                notice.ParseFromString(m.message());
                spdlog::info("tid: {} 是当前回合玩家, 剩余时间: {}ms", tid, notice.remainingtime());
            } else if (m.type() == NEW_CARD) {
                NewCard notice;
                notice.ParseFromString(m.message());
                spdlog::info("tid: {} 拿到新牌, 牌数: {}", tid, notice.newcards_size());
                for (int i = 0; i < notice.newcards_size(); ++i) {
                    spdlog::info("tid: {} 牌面 id: {}, type: {}",
                                 tid, notice.newcards(i).id(), CardName[notice.newcards(i).type()]);
                    cards.push_back(GET_CARD(notice.newcards(i).id(), notice.newcards(i).type()));
                }
            } else if (m.type() == DISCARD_CARD) {
                DiscardCard notice;
                notice.ParseFromString(m.message());
                spdlog::info("tid: {} 被迫弃牌, 牌数: {}", tid, notice.discardedcards_size());
                for (int i = 0; i < notice.discardedcards_size(); ++i) {
                    spdlog::info("tid: {} 牌面 id: {}, type: {}",
                                 tid, notice.discardedcards(i).id(), CardName[notice.discardedcards(i).type()]);
                }
            }
        }
    }

    void card_play(size_t num, size_t target) {
        if (num >= cards.size()) {
            spdlog::error("tid: {} 想要出的牌数: {} 大于手牌数: {}", tid, num, cards.size());
            return;
        }
        uint64_t c = cards[num];
        spdlog::info("tid: {} 出牌, 牌面 id: {}, type: {}", tid, GET_ID(c), CardName[GET_TYPE(c)]);
        ActionPlay action;
        action.set_targetplayerid(target);
        action.mutable_card()->set_id(GET_ID(c));
        action.mutable_card()->set_type(CardType_pb(GET_TYPE(c)));
        BasicMessage m;
        m.set_type(ACTION_PLAY);
        m.set_message(action.SerializeAsString());
        zmq::message_t req(m.ByteSizeLong());
        m.SerializeToArray(req.data(), req.size());
        socket_pair.send(req, zmq::send_flags::none);
        cards.erase(cards.begin() + num);
    }

    void print_cards() {
        for (auto &card : cards) {
            spdlog::info("tid: {} 牌面 id: {}, type: {}", tid, GET_ID(card), CardName[GET_TYPE(card)]);
        }
    }
};


int main() {
    spdlog::set_level(spdlog::level::debug);
    zmq::context_t context(1);
    std::vector<std::shared_ptr<Client> > clients;
    while (true) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "add") {
            spdlog::info("添加客户端 tid: {}", clients.size());
            clients.emplace_back(std::make_shared<Client>(context, clients.size()));
        } else if (cmd == "list") {
            size_t id;
            std::cin >> id;
            if (id >= clients.size()) {
                spdlog::error("tid: {} 不存在", id);
            } else {
                clients[id]->print_cards();
            }
        } else if (cmd == "play") {
            size_t id, num, target, cnum;
            std::cin >> id >> num >> target;
            if (id >= clients.size()) {
                spdlog::error("tid: {} 不存在", id);
            } else {
                clients[id]->card_play(num, target);
            }
        }
    }
    return 0;
}