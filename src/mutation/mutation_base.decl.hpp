/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#ifndef GA_MUTATION_BASE_DECL_HPP
#define GA_MUTATION_BASE_DECL_HPP

#include "mutation_base.fwd.hpp"
#include "../utility/probability.hpp"
#include <vector>
#include <utility>

namespace genetic_algorithm
{
    class GaInfo;

} // namespace genetic_algorithm

/** Mutation operators used in the algorithms. */
namespace genetic_algorithm::mutation
{
    /**
    * Base class used for all of the mutation operators. \n
    * Every mutation operator takes a Candidate, and mutates that candidate using a set probability. \n
    * (This probability can either be per-candidate or per-gene depending on how the particular operator is defined.) \n
    * The mutation is allowed change the length of the candidate's chromosome.
    */
    template<Gene T>
    class Mutation
    {
    public:
        using GeneType = T;

        /**
        * Create a mutation operator.
        *
        * @param pm The mutation probability. Must be in the closed interval [0.0, 1.0].
        */
        explicit Mutation(Probability pm) noexcept :
            pm_(pm) {}

        Mutation(const Mutation&)               = default;
        Mutation(Mutation&&)                    = default;
        Mutation& operator=(const Mutation&)    = default;
        Mutation& operator=(Mutation&&)         = default;
        virtual ~Mutation()                     = default;

        /**
        * Set the mutation rate used by the operator.
        * 
        * @param pm The mutation probability. Must be in the closed interval [0.0, 1.0].
        */
        void mutation_rate(Probability pm) { pm_ = pm; };

        /** @returns The mutation rate set for the operator. */
        [[nodiscard]]
        Probability mutation_rate() const noexcept { return pm_; }

        /**
        * Perform mutation on a candidate using the set mutation rate.
        *
        * @param ga The genetic algorithm the crossover operator is being used in.
        * @param candidate The candidate to mutate.
        */
        void operator()(const GaInfo& ga, Candidate<T>& candidate) const;

    private:

        /* The actual implementation of the mutation function. Performs the mutation using the set probability.
           This function should only change the chromosome of the candidate. */
        virtual void mutate(const GaInfo& ga, Candidate<T>& candidate) const = 0;

        Probability pm_;
    };

} // namespace genetic_algorithm::mutation

#endif // !GA_MUTATION_BASE_DECL_HPP