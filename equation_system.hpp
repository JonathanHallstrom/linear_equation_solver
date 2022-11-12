#pragma once

#include "equation.hpp"

#include <vector>
#include <utility>
#include <iostream>
#include <iomanip>

struct equation_system {
    std::vector<equation> equations;

    equation_system() = default;

    template<class... Args>
    explicit equation_system(Args &&... args): equations{std::forward<Args>(args)...} {
        simplify_equations();
    }

    explicit equation_system(std::vector<equation> eqs) : equations{std::move(eqs)} {
        simplify_equations();
    }

    void simplify_equations() {
        for (auto &i: equations) i.simplify();
    }

    /**
     * put the matrix A in row-echelon form
     * @source https://en.wikipedia.org/wiki/Gaussian_elimination#Pseudocode
     */
    static void triangularize(std::vector<std::vector<fraction>> &A) {
        const usize M = std::size(A);
        const usize N = std::size(A.front());

        usize h = 0, k = 0;
        while (h < M && k < N) {
            /* Find the k-th pivot: */
            const usize i_max = argmax(h, M, [&A, k](usize idx) { return A[idx][k].abs(); });

            if (A[i_max][k] == 0) {
                /* No pivot in this column, pass to next column */
                ++k;
            } else {
                std::swap(A[h], A[i_max]);
                for (usize i = h + 1; i < M; ++i) {
                    const fraction f = A[i][k] / A[h][k];

                    /* Fill with zeros the lower part of pivot column: */
                    A[i][k] = 0;

                    /* Do for all remaining elements in current row: */
                    for (usize j = k + 1; j < N; ++j)
                        A[i][j] -= A[h][j] * f;
                }
            }
            ++h, ++k;
        }
    }

    std::map<std::string, fraction> solve() const {
        using std::vector, std::string;
        std::map<string, i32> name_indices;
        vector<string> names_by_index;

        /**
         * put all the variable names in convenient containers
         */
        {
            for (auto &eq: equations)
                for (auto &[name, _]: eq.variable_terms)
                    name_indices[name] = 0;
            i32 idx = 0;
            names_by_index.reserve(std::size(name_indices));
            for (auto &[name, i]: name_indices) {
                names_by_index.emplace_back(name);
                i = idx++;
            }
        }

        const usize M = std::size(equations);
        const usize N = std::size(names_by_index) + 1;

        assert(N - 1 <= M && "system of equations is underspecified");

        // MxN matrix
        vector<vector<fraction>> A(M, vector<fraction>(N));
        for (usize i = 0; i < M; ++i) {
            for (usize j = 0; j + 1 < N; ++j) {
                A[i][j] = equations[i][names_by_index[j]].simplified();
            }
            A[i].back() = equations[i].constant_term.simplified();
        }

        triangularize(A);

        std::map<string, fraction> result;
        vector<fraction> values(N - 1);

        for (usize i = M; i-- > 0;) {
            usize unknown_idx = 0;
            bool found_unknown = false, found_several_unknown = false;
            for (usize j = 0; j + 1 < N; ++j) {
                if (!A[i][j])
                    continue;

                if (values[j]) {
                    A[i].back() -= A[i][j] * values[j];
                    A[i][j] = 0;
                } else if (!found_unknown) {
                    found_unknown = true;
                    unknown_idx = j;
                } else {
                    found_several_unknown = true;
                    break;
                }
            }

            if (found_several_unknown)
                throw std::runtime_error("something is seriously wrong, several unknown variables");

            if (found_unknown) {
                A[i].back() /= A[i][unknown_idx];
                values[unknown_idx] = A[i].back();
            }
        }

        for (usize i = 0; i < N - 1; ++i)
            result[names_by_index[i]] = values[i];

        return result;
    }
};