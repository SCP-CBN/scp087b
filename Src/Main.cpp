#include <PGE/Init/Init.h>

#include <thread>
#include <chrono>

#include "World/World.h"
#include "World/StatWorld.h"

using namespace PGE;

int PGE::Init::main(const std::vector<String>& args) {
    TimeMaster master;

    std::unique_ptr<StatWorld> statWorld = std::make_unique<StatWorld>(master);

    World theWorld(master);
    
    std::thread statThread([&]() {
        while (!statWorld->shouldEnd() && !theWorld.shouldEnd()) {
            using namespace std::chrono_literals;
            statWorld->run();
            std::this_thread::sleep_for(200ms);
        }
        statWorld.release();
    });
    
    while (!theWorld.shouldEnd()) {
        theWorld.run();
    }
    
    statThread.join();

    return 0;
}
