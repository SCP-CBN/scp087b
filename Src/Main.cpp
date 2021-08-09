#include <PGE/Init/Init.h>

#include "World/World.h"

using namespace PGE;

int PGE::Init::main(const std::vector<String>& args) {
    World theWorld;
    while (!theWorld.shouldEnd()) {
        theWorld.run();
    }
    return 0;
}
