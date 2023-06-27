
#ifndef KINGDOMCARD_PLAYER_H
#define KINGDOMCARD_PLAYER_H

#include <vector>
#include <memory>
#include <zmq.hpp>
#include "basic/Card.h"

namespace kc {
    enum PlayerIdentity {
        UNKNOWN,
        LOYAL,
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
        bool isAlive;
        uint16_t health;
        uint16_t maxHealth;
        std::vector<CardPtr> handCards;

    public:
        uint16_t const id;
        zmq::socket_t socket;
        std::mutex mtx;

        void static init() {
            idCounter = 0;
        }

        Player(uint16_t id, zmq::socket_t socket)
                : identity(UNKNOWN), isAlive(true), health(4), maxHealth(4), id(id), socket(std::move(socket)) {}

        [[nodiscard]] bool getIsAlive() const { return isAlive; }

        [[nodiscard]] uint16_t getHealth() const { return health; }

        [[nodiscard]] uint16_t getMaxHealth() const { return maxHealth; }

        [[nodiscard]] PlayerIdentity getIdentity() const { return identity; }

        void setIsAlive(bool is_alive) { isAlive = is_alive; }

        void setHealth(uint16_t n_health) { health = n_health; }

        void setMaxHealth(uint16_t max_health) { maxHealth = max_health; }

        void setIdentity(PlayerIdentity n_identity) { identity = n_identity; }
    };
}

#endif //KINGDOMCARD_PLAYER_H
