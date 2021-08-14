#ifndef B_TIME_H_INCLUDED
#define B_TIME_H_INCLUDED

#include <PGE/String/Key.h>

struct Time {
    PGE::u64 time;
    Time* parent;
    std::unordered_map<PGE::String::RedundantKey, Time> children;
    void print(PGE::String& ret, PGE::u64 parentTime, int depth, const PGE::String& name) const;
};

#endif // B_TIME_H_INCLUDED