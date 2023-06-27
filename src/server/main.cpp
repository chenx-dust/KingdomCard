#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include "communication/GameServer.h"

int main()
{
    zmq::context_t context(1);
    kc::GameServer server(context, 13364);
    server.waitForConnection();
    spdlog::info("等待连接成功");
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        server.checkAndKick();
        if (server.isReady()) {
//            server.start();
            break;
        }
    }
    return 0;
}
