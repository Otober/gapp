/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#include "integer.hpp"
#include "crossover_dtl.hpp"
#include "../utility/rng.hpp"
#include <stdexcept>

namespace genetic_algorithm::crossover::integer
{
    auto SinglePoint::crossover(const GaInfo&, const Candidate<size_t>& parent1, const Candidate<GeneType>& parent2) const -> CandidatePair<GeneType>
    {
        return dtl::nPointCrossoverImpl(parent1, parent2, 1);
    }

    auto TwoPoint::crossover(const GaInfo&, const Candidate<size_t>& parent1, const Candidate<GeneType>& parent2) const -> CandidatePair<GeneType>
    {
        return dtl::nPointCrossoverImpl(parent1, parent2, 2);
    }

    NPoint::NPoint(size_t n)
    {
        num_crossover_points(n);
    }

    void NPoint::num_crossover_points(size_t n)
    {
        if (n == 0)
        {
            throw std::invalid_argument("The number of crossover points must be at least 1 for the n-point crossover.");
        }

        n_ = n;
    }

    auto NPoint::crossover(const GaInfo&, const Candidate<GeneType>& parent1, const Candidate<GeneType>& parent2) const -> CandidatePair<GeneType>
    {
        return dtl::nPointCrossoverImpl(parent1, parent2, n_);
    }

    Uniform::Uniform(double ps)
    {
        swap_probability(ps);
    }

    void Uniform::swap_probability(double ps)
    {
        if (!(0.0 <= ps && ps <= 1.0))
        {
            throw std::invalid_argument("The swap probability must be in the range [0.0, 1.0] for the uniform crossover.");
        }

        ps_ = ps;
    }

    auto Uniform::crossover(const GaInfo&, const Candidate<GeneType>& parent1, const Candidate<GeneType>& parent2) const -> CandidatePair<GeneType>
    {
        size_t chrom_len = parent1.chromosome.size();

        if (parent2.chromosome.size() != chrom_len)
        {
            throw std::invalid_argument("The parent chromosomes must be the same length for the uniform crossover.");
        }

        Candidate child1{ parent1 }, child2{ parent2 };

        for (size_t i = 0; i < chrom_len; i++)
        {
            if (rng::randomReal() <= ps_)
            {
                child1.chromosome[i] = parent2.chromosome[i];
                child2.chromosome[i] = parent1.chromosome[i];
            }
        }

        return { child1, child2 };
    }

} // namespace genetic_algorithm::crossover::integer