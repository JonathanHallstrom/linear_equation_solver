#pragma once

#include <ranges>

#define WRAP_OVERLOADS(x) []<class... T>(T &&... params) { return x(std::forward<T>(params)...); }

template<class ArgType>
requires std::integral<ArgType>
constexpr ArgType argmax(ArgType lo, ArgType hi, auto &&function) {
    ArgType res = lo;
    auto comp = function(lo);
    while (++lo < hi) {
        const auto val = function(lo);
        if (val > comp) {
            comp = val;
            res = lo;
        }
    }
    return res;
}