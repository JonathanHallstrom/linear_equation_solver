#pragma once

#include "fraction.hpp"
#include "utils.hpp"

#include <map>
#include <cassert>
#include <numeric>
#include <string>

struct equation {
    std::map<std::string, fraction> variable_terms;
    fraction constant_term{};

    equation() = default;

    equation(equation const &) = default;

    equation(equation &&) noexcept = default;

    template<class... T>
    explicit equation(T &&... coeffs, fraction constant): variable_terms{std::forward<T>(coeffs)...},
                                                          constant_term{constant} { simplify(); }

    equation &operator=(equation const &) = default;

    [[nodiscard]] std::string to_string() const {
        auto copy = *this;
        copy.simplify();
        std::string result;
        result.reserve(copy.variable_terms.size() * 5);
        bool first = true;
        for (auto &[name, coeff]: copy.variable_terms) {
            if (!first) result.append(coeff > 0 ? " + " : " - ");
            if (coeff.abs() != fraction{1}) {
                if (first)
                    result.append(coeff.to_string()).append(name);
                else
                    result.append(coeff.abs().to_string()).append(name);
            } else {
                result.append(name);
            }
            first = false;
        }
        result.append(" = ").append(constant_term.to_string());
        return result;
    }

    fraction operator[](std::string const &name) const {
        if (variable_terms.contains(name)) {
            return variable_terms.at(name);
        } else {
            return fraction{};
        }
    }

    fraction &operator[](std::string const &name) {
        return variable_terms[name];
    }

    // simplifies this equation and returns a reference to *this
    void simplify() {
        for (auto &[name, coeff]: variable_terms) coeff.simplify();
        erase_if(variable_terms, [](auto &kvp) { return kvp.second == fraction{0}; });
        if (variable_terms.size() == 1) {
            constant_term /= variable_terms.begin()->second;
            variable_terms.begin()->second = 1;
        }
    }

    // returns simplified copy
    [[nodiscard]] equation simplified() const {
        auto res = *this;
        res.simplify();
        return res;
    }

    // adds passed in equation and simplifies, returns reference
    equation &operator+=(equation const &other) {
        assert(variable_terms.size() == other.variable_terms.size());
        for (auto &[name, coeff]: other.variable_terms) {
            variable_terms[name] += coeff;
        }
        constant_term += other.constant_term;
        return *this;
    }

    // returns simplified equation equal to the sum of the equations
    equation operator+(equation const &other) const {
        auto result = *this;
        result += other;
        return result;
    }
};