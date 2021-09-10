#include <PGE/Init/Init.h>

#include <thread>
#include <chrono>

#include "World/World.h"
#include "World/StatWorld.h"

using namespace PGE;

class ScopedThread {
    public:
        ScopedThread(std::thread&& t) {
            thread = std::move(t);
        }

        ~ScopedThread() {
            if (thread.joinable()) {
                thread.join();
            }
        }

    private:
        std::thread thread;
};

int PGE::Init::main(const std::vector<String>& args) {
    TimeMaster master;

    StatWorld statWorld(master);
    World theWorld(master);

    ScopedThread _ = std::thread([&]() {
        while (!statWorld.shouldEnd() && !theWorld.shouldEnd()) {
            using namespace std::chrono_literals;
            statWorld.run();
            std::this_thread::sleep_for(200ms);
        }
    });
    
    theWorld.run();

    return 0;
}
