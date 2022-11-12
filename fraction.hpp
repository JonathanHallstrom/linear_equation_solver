#pragma once

#include "types.hpp"
#include <string_view>
#include <string>

struct fraction {
    i64 numerator = 0, denominator = 1;

    constexpr fraction() = default;

    constexpr fraction(i64 num) : numerator{num}, denominator{1} {}

    constexpr fraction(i64 num, i64 denom) : numerator{num}, denominator{denom} {}

    constexpr fraction(fraction const &) = default;

    constexpr fraction &operator=(fraction const &) = default;

    constexpr fraction(std::string_view s) : numerator{}, denominator{} {
        bool found_slash = false;
        for (auto i: s) {
            if (!(('0' <= i && i <= '9') || i == '/')) continue;
            if (i == '/') {
                found_slash = true;
            } else {
                i64 &n = (found_slash ? denominator : numerator);
                n = 10 * n + i - '0';
            }
        }
        if (!found_slash) denominator = 1;
        simplify();
    }

    constexpr bool operator==(fraction other) const {
        other.simplify();
        return numerator == other.numerator && denominator == other.denominator;
    }

    [[nodiscard]] std::string to_string() const {
        if (denominator == 1) {
            return std::to_string(numerator);
        } else {
            return std::to_string(numerator) + "/" + std::to_string(denominator);
        }
    }

    constexpr fraction &operator/=(i64 x) {
        if (numerator % x == 0)
            numerator /= x;
        else
            denominator *= x, simplify();
        return *this;
    }

    constexpr fraction operator/(i64 x) const {
        fraction result = *this;
        return result /= x;
    }

    constexpr fraction &operator*=(i64 x) {
        if (denominator % x == 0)
            denominator /= x;
        else
            numerator *= x, simplify();
        return *this;
    }

    constexpr fraction operator*(i64 x) const {
        fraction result = *this;
        return result *= x;
    }

    constexpr fraction &operator-=(i64 x) {
        return *this += -x;
    }

    constexpr fraction operator-(i64 x) {
        return *this + -x;
    }

    constexpr fraction &operator+=(i64 x) {
        numerator += x * denominator;
        simplify();
        return *this;
    }

    constexpr fraction operator+(i64 x) {
        fraction result = *this;
        return result += x;
    }

    constexpr fraction &operator/=(fraction x) {
        return *this *= fraction{x.denominator, x.numerator};
    }

    constexpr fraction operator/(fraction x) const {
        fraction result = *this;
        return result /= x;
    }

    constexpr fraction operator-() const {
        fraction result = *this;
        result.numerator *= -1;
        return result;
    }

    constexpr fraction &operator*=(fraction other) {
        numerator *= other.numerator;
        denominator *= other.denominator;
        simplify();
        return *this;
    }

    constexpr fraction operator*(fraction other) const {
        fraction result = *this;
        return result *= other;
    }

    constexpr fraction &operator+=(fraction other) {
        return *this = *this + other;
    }

    constexpr fraction operator+(fraction other) const {
        fraction result = other;
        const i64 g = _gcd(denominator, other.denominator);
        result.denominator *= denominator / g;
        result.numerator *= denominator / g;
        result.numerator += numerator * other.denominator / g;
        return result;
    }

    constexpr fraction &operator-=(fraction other) {
        return *this = *this + -other;
    }

    constexpr fraction operator-(fraction other) const {
        return *this + -other;
    }

    constexpr std::strong_ordering operator<=>(fraction other) const {
        return static_cast<i128>(numerator) * other.denominator <=>
               static_cast<i128>(denominator) * other.numerator;
    }

    constexpr explicit operator bool() const {
        return numerator;
    }

    constexpr void simplify() {
        if (denominator == 0) {
            numerator = 0, denominator = 1;
            return;
        }
        const i64 g = _gcd(numerator, denominator);
        numerator /= g;
        denominator /= g;
        if (denominator < 0)
            numerator *= -1, denominator *= -1;
    }

    [[nodiscard]] constexpr fraction simplified() const {
        fraction result = *this;
        result.simplify();
        return result;
    }

    [[nodiscard]] constexpr fraction abs() const {
        return {numerator > 0 ? numerator : -numerator,
                denominator > 0 ? denominator : -denominator};
    }

private:
    constexpr static i64 _gcd(i64 a, i64 b) {
        while (a) {
            const auto c = a;
            a = b % c;
            b = c;
        }
        return b;
    }
};
