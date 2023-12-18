
#ifndef KINGDOMCARD_GAMECONTROLLER_H
#define KINGDOMCARD_GAMECONTROLLER_H

#include <any>
#include <chrono>
#include <vector>
#include <memory>
#include <set>
#include "basic/Player.h"
#include "basic/Card.h"
#include "basic/Utility.h"
#include "basic_message.pb.h"

namespace kc {

    const std::chrono::microseconds TURN_TIME_LIMIT = std::chrono::seconds(30);
    const std::chrono::microseconds REACT_TIME_LIMIT = std::chrono::seconds(5);

    class CardAction {
    public:
        CardAction(size_t card_id, CardType type, size_t source_id, size_t target_id, size_t draw_card_num) :
                card_id(card_id), type(type), source_id(source_id), target_id(target_id), draw_card_num(draw_card_num) {}
        CardAction(size_t card_id, CardType type, size_t source_id, size_t target_id) :
                card_id(card_id), type(type), source_id(source_id), target_id(target_id), draw_card_num(-1) {}
        CardAction(size_t card_id, CardType type, size_t source_id) :
                card_id(card_id), type(type), source_id(source_id), target_id(-1), draw_card_num(-1) {}
        size_t const card_id;
        CardType const type;
        size_t const source_id;
        size_t const target_id;
        size_t const draw_card_num;
    };

    class DiscardAction {
    public:
        DiscardAction(size_t player_id, std::set<size_t> card_ids) :
                player_id(player_id), card_ids(std::move(card_ids)) {}
        size_t const player_id;
        std::set<size_t> const card_ids;
    };

    class GameController {
    private:
        bool isStarted = false;
        size_t currIdx = 0;
        size_t lordId = -1;
        std::vector<PlayerPtr> &players;
        std::vector<CardPtr> cards;
        util::Timer turn_timer;

        void init();

        void startCommand();

        void broadcast(CommandType commandType, const std::string &msg);

        [[nodiscard]] std::vector<size_t> getPlayerList(bool exclude_current = false) const;

        size_t nextPlayerIdx();

        void bcStatus();

        bool checkWin();

        [[nodiscard]] CardPtr drawCard();

        void newTurn();

        [[nodiscard]] Player& findPlayerById(size_t id);

        [[nodiscard]] std::any waitForCard(const std::vector<size_t> &target);

        void bcCard(const CardAction& action);

        [[nodiscard]] std::optional<CardAction> waitForReact(const std::vector<size_t> &target, const std::set<CardType> &type);

        void dealWithCard(const CardAction& action);

        [[nodiscard]] bool isNearby(size_t target_id);

        void removeCard(size_t player_id, size_t card_id, std::optional<CardType> type_check = std::nullopt);

        void removeCard(const CardAction& action);

        void damage(size_t player_id, size_t damage = 1);

    public:
        explicit GameController(std::vector<PlayerPtr> &players) : players(players) {}

        void start();
    };
}

#endif //KINGDOMCARD_GAMECONTROLLER_H
