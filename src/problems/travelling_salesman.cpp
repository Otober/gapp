/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#include "travelling_salesman.hpp"
#include <vector>
#include <span>
#include <string>
#include <cmath>
#include <cstddef>

namespace genetic_algorithm::problems
{
    TSP::TSP(std::span<const Coords> cities, double optimal_value) :
        BenchmarkFunction("TSP" + std::to_string(cities.size()), cities.size() - 1, 1, Bounds{ 0, cities.size() - 2 }),
        distance_matrix_(cities.size(), cities.size(), 0.0)
    {
        optimal_value_ = { optimal_value };
        ideal_point_ = optimal_value_;
        nadir_point_ = optimal_value_;

        for (size_t i = 0; i < distance_matrix_.nrows(); i++)
        {
            for (size_t j = 0; j < distance_matrix_.ncols(); j++)
            {
                const double dx = cities[i][0] - cities[j][0];
                const double dy = cities[i][1] - cities[j][1];

                distance_matrix_[i][j] = std::hypot(dx, dy);
            }
        }
    }

    auto TSP::invoke(const std::vector<PermutationGene>& chrom) const -> FitnessVector
    {
        const size_t ncities = distance_matrix_.size();

        double distance = 0.0;
        for (size_t i = 0; i < num_vars() - 1; i++)
        {
            distance += distance_matrix_[chrom[i]][chrom[i + 1]];
        }
        distance += distance_matrix_[ncities - 1][chrom.front()];
        distance += distance_matrix_[ncities - 1][chrom.back()];

        return { -distance }; /* For maximization. */
    }

} // namespace genetic_algorithm::problems