#include "Timer.h"

#include "TimeMaster.h"

using namespace PGE;

Timer::Timer(TimeMaster& tm, const String& name) : name(name), start(Clock::now()) {
    auto it = tm.current->children.find(name);
    if (it == tm.current->children.end()) {
        tm.current = &tm.current->children.emplace(name, Time{ 0, tm.current }).first->second;
    } else {
        tm.current = &it->second;
    }
    current = &tm.current;
}

Timer::~Timer() {
    (*current)->time += (Clock::now() - start).count();
    *current = (*current)->parent;
}
