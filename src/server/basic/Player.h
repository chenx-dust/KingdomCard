
#ifndef KINGDOMCARD_PLAYER_H
#define KINGDOMCARD_PLAYER_H

#include <vector>
#include <memory>
#include <set>
#include <zmq.hpp>
#include "basic/Card.h"

namespace kc {
    size_t const MAX_PLAYER_NUM = 10;
    size_t const MIN_PLAYER_NUM = 4;
    size_t const ID_COUNT[7][4] = {
            {1, 1, 1, 1},
            {1, 1, 2, 1},
            {1, 1, 3, 1},
            {1, 2, 3, 1},
            {1, 2, 4, 1},
            {1, 3, 4, 1},
            {1, 3, 4, 2}
    };

    std::string const PlayerIdentityName[5] = {
            "未知",
            "主公",
            "忠臣",
            "反贼",
            "内奸"
    };

    enum PlayerIdentity {
        UNKNOWN,
        LORD,
        MINISTER,
        REBEL,
        SPY
    };

    class Player;

    typedef std::shared_ptr<Player> PlayerPtr;

    class Player {
    private:
        uint16_t static idCounter;
        PlayerIdentity identity;
        bool alive;
        uint16_t health;
        uint16_t maxHealth;
        std::vector<CardPtr> handCards;

    public:
        uint16_t const id;
        zmq::socket_t socket;
        std::mutex mtx;

        Player(uint16_t id, zmq::socket_t socket)
                : identity(UNKNOWN), alive(true), health(4), maxHealth(4), id(id), socket(std::move(socket)) {}

        [[nodiscard]] bool isAlive() const { return alive; }

        [[nodiscard]] uint16_t getHealth() const { return health; }

        [[nodiscard]] uint16_t getMaxHealth() const { return maxHealth; }

        [[nodiscard]] PlayerIdentity getIdentity() const { return identity; }

        void setHealth(uint16_t n_health) { health = n_health; }

        void setMaxHealth(uint16_t max_health) { maxHealth = max_health; }

        void setIdentity(PlayerIdentity n_identity) { identity = n_identity; }

        [[nodiscard]] size_t getCardCount() const { return handCards.size(); }

        void addCard(CardPtr &&card) { handCards.emplace_back(std::move(card)); }

        void newCardList(std::vector<CardPtr> &&cards);

        [[nodiscard]] const std::vector<CardPtr> &getCards() const { return handCards; }

        [[nodiscard]] CardPtr &getCard(size_t cid);

        [[nodiscard]] CardPtr removeCard(size_t cid);

        [[nodiscard]] CardPtr removeCardByNum(size_t num);

        [[nodiscard]] std::vector<CardPtr> discardMoreCard();

        [[nodiscard]] std::vector<CardPtr> die();

        [[nodiscard]] bool hasCard(CardType type);

        [[nodiscard]] bool hasCard(const std::set<CardType>& type);
    };
}

#endif //KINGDOMCARD_PLAYER_H
