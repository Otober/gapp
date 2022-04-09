/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#ifndef GA_GA_BASE_DECL_HPP
#define GA_GA_BASE_DECL_HPP

#include "../population/candidate.hpp"
#include "../utility/concepts.hpp"
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <utility>
#include <functional>
#include <type_traits>
#include <atomic>
#include <cstddef>
#include <memory>
#include <concepts>


namespace genetic_algorithm
{
    namespace selection
    {
        class Selection;

        template<typename T>
        concept SelectionMethod = requires
        {
            detail::DerivedFrom<T, Selection>;
            std::copy_constructible<T>;
        };
    }
    namespace crossover
    {
        template<Gene T>
        class Crossover;

        template<typename T>
        concept CrossoverMethod = requires
        {
            detail::DerivedFromSpecializationOf<T, Crossover>;
            std::copy_constructible<T>;
        };
    }
    namespace mutation
    {
        template<Gene T>
        class Mutation;

        template<typename T>
        concept MutationMethod = requires
        {
            detail::DerivedFromSpecializationOf<T, Mutation>;
            std::copy_constructible<T>;
        };
    }
    namespace stopping
    {
        class StopCondition;

        template<typename T>
        concept StopMethod = requires
        {
            detail::DerivedFrom<T, StopCondition>;
            std::copy_constructible<T>;
        };
    }

    class GaInfo
    {
    public:
        GaInfo(size_t chrom_len);

        GaInfo(const GaInfo&)               = default;
        GaInfo(GaInfo&&)                    = default;
        GaInfo& operator=(const GaInfo&)    = default;
        GaInfo& operator=(GaInfo&&)         = default;
        virtual ~GaInfo()                   = default;

        /**
        * Should be set to false if the fitness function does not change over time. \n
        * (The fitness function will always return the same fitness values for a given chromosome.) \n
        * Used to eliminate unnecesary objective function evaluations.
        */
        bool dynamic_fitness = false;

        /**
        * All pareto optimal optimal solutions found by the algorithm will be stored in the solutions,
        * not just the ones in the current population if set to true. \n
        */
        bool archive_optimal_solutions = false;

        /**
        * Sets the length of the chromosomes (number of genes) of the Candidate solutions used in the algorithm to @p len. \n
        * The chromosome length must be at least 1.
        *
        * @param len The length of the chromosomes.
        */
        void chrom_len(size_t len);

        /** @returns The chromosome length used for the candidates of the population. */
        [[nodiscard]] size_t chrom_len() const noexcept;

        /**
        * Sets the number of Candidates used in the population to @p size. \n
        * The population size must be at least 1.
        *
        * @param size The size of the populations.
        */
        void population_size(size_t size);

        /** @returns The population size of the algorithm. */
        [[nodiscard]] size_t population_size() const noexcept;
        
        /**
        * Sets the maximum number of generations the algorithm runs for to @p max_gen. The
        * algorithm will always stop when this generation has been reached regardless of what stop
        * condition was set, but it can stop earlier when another stop condition is selected.
        * @see stop_condition @see StopCondition \n
        * The value of @p max_gen must be at least 1.
        *
        * @param max_gen The maximum number of generations.
        */
        void max_gen(size_t max_gen);

        /** @returns The maximum number of generations set for the algorithm. */
        [[nodiscard]] size_t max_gen() const noexcept;

        /** @returns The number of objectives. Determined by the algorithm and returns 0 before the start of the algorithm. */
        [[nodiscard]] size_t num_objectives() const noexcept;

        /** @returns The fitness matrix of the population. */
        [[nodiscard]]
        virtual std::vector<std::vector<double>> fitness_matrix() const = 0;

        /** @returns The number of fitness evaluations performed so far by the algorithm. */
        [[nodiscard]] size_t num_fitness_evals() const noexcept;

        /** @returns The current value of the generation counter. */
        [[nodiscard]] size_t generation_cntr() const noexcept;

    protected:

        size_t generation_cntr_ = 0;
        size_t num_objectives_ = 0;
        std::atomic<size_t> num_fitness_evals_ = 0;

        size_t chrom_len_;
        size_t population_size_ = 100;
        size_t max_gen_ = 500;
    };

    /**
    * Base GA class.
    *
    * @tparam geneType The type of the genes in the candidates' chromosomes.
    */
    template<typename geneType>
    class GA : public GaInfo
    {
    public:
        using GeneType = geneType;
        using Candidate = Candidate<GeneType>;  /**< The candidates used in the algorithm, each representing a solution to the problem. */

        using Chromosome = std::vector<GeneType>;                                           /**< . */
        using CandidatePair = CandidatePair<GeneType>;                                      /**< . */
        using CandidateVec = std::vector<Candidate>;                                        /**< . */
        using CandidateSet = std::unordered_set<Candidate, CandidateHasher<GeneType>>;      /**< . */
        using Population = std::vector<Candidate>;                                          /**< . */

        using FitnessFunction = std::function<std::vector<double>(const Chromosome&)>;      /**< The type of the fitness function. */
        using SelectionFunction = std::function<Candidate(const Population&)>;              /**< The type of the selection function. */
        using MutationFunction = std::function<void(Candidate&, double)>;                   /**< The type of the mutation function. */
        using RepairFunction = std::function<Chromosome(const Chromosome&)>;                /**< The type of the repair function. */
        using CallbackFunction = std::function<void(const GA&)>;

        /**
        * The repair function applied to each Candidate of the population after the mutations if it isn't a nullptr. \n
        * This can be used to perform local search after the mutations, implementing a memetic algorithm.
        */
        RepairFunction repairFunction = nullptr;

        CallbackFunction endOfGenerationCallback = nullptr;

        /**
        * Standard constructor for the GA.
        *
        * @param chrom_len The length of the chromosomes (number of genes).
        * @param fitness_function The fitness function to find the maximum of.
        */
        GA(size_t chrom_len, FitnessFunction fitness_function);

        virtual ~GA() = default;

        /**
        * Runs the genetic algorithm with the selected settings.
        *
        * @returns The optimal solutions.
        */
        [[maybe_unused]]
        const CandidateVec& run();

        /** @returns A vector of the pareto optimal solutions found while running the algorithm. */
        [[nodiscard]]
        const CandidateVec& solutions() const;

        /** @returns The population of the final generation in the algorithm. */
        [[nodiscard]]
        const Population& population() const;

        [[nodiscard]]
        std::vector<std::vector<double>> fitness_matrix() const override final;

        /**
        * Sets the initial population to be used in the algorithm to @p pop instead of randomly generating it. \n
        * If @p pop is empty, the initial population will be randomly generated. \n
        * If the preset population's size is not equal to the population size set, either additional randomly generated
        * Candidates will be added to fill out the initial population, or some Candidates will be discarded from the preset.
        *
        * @param pop The initial population to use in the algorithm.
        */
        void initial_population(const Population& pop);

        /**
        * Sets the fitness function used by the algorithm to @p f. \n
        * The fitness function should return a vector whose size is equal to the number of objectives, and
        * each element of the vector should be finite.
        *
        * @param f The fitness function to find the maximum of.
        */
        void fitness_function(FitnessFunction f);

        /* SELECTION METHOD */
        /**
        * Sets the selection method used in the single-objective algorithm to @p method. \n
        * The selection method set is ignored in the other algorithm types. @see Mode
        *
        * @param method The selection method used in the single-objective algorithm.
        */
        template<selection::SelectionMethod F>
        void selection_method(const F& f);

        void selection_method(std::unique_ptr<selection::Selection>&& f);

        template<selection::SelectionMethod F = selection::Selection>
        [[nodiscard]] F& selection_method() const;

        /* CROSSOVER */
        template<crossover::CrossoverMethod F>
        void crossover_method(const F& f);

        void crossover_method(std::unique_ptr<crossover::Crossover<GeneType>>&& f);

        template<crossover::CrossoverMethod F = crossover::Crossover<GeneType>>
        F& crossover_method() const;

        /* MUTATION */
        template<mutation::MutationMethod F>
        void mutation_method(const F& f);

        void mutation_method(std::unique_ptr<mutation::Mutation<GeneType>>&& f);

        template<mutation::MutationMethod F = mutation::Mutation<GeneType>>
        F& mutation_method() const;

        /* STOP CONDITION */
        /* The algorithm always stops when the set max_gen generation has been reached regardless of the stop condition set. */
        template<stopping::StopMethod F>
        void stop_condition(const F & f);

        void stop_condition(std::unique_ptr<stopping::StopCondition> && f);

        template<stopping::StopMethod F = stopping::StopCondition>
        [[nodiscard]] F& stop_condition() const;

    protected:

        Population population_;
        CandidateVec solutions_;
        Population initial_population_preset_;

        FitnessFunction fitness_function_;
        std::unique_ptr<selection::Selection> selection_;
        std::unique_ptr<crossover::Crossover<GeneType>> crossover_;
        std::unique_ptr<mutation::Mutation<GeneType>> mutation_;
        std::unique_ptr<stopping::StopCondition> stop_condition_;

        void init();
        virtual Candidate generateCandidate() const = 0;
        Population generateInitialPopulation() const;
        void evaluate(Population& pop);
        Population nextPopulation(Population& pop, Population& children) const;
        void updateOptimalSolutions(CandidateVec& optimal_sols, const Population& pop) const;
        void repair(Population& pop) const;
        bool stopCondition() const;

    };

    /** Genetic algorithm types. */
    template<typename T>
    concept GeneticAlgorithm = detail::DerivedFromSpecializationOf<T, GA>;

} // namespace genetic_algorithm

#endif // !GA_GA_BASE_DECL_HPP