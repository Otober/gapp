/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#include "nsga3.hpp"
#include "nd_sort.hpp"
#include "../core/ga_info.hpp"
#include "../population/population.hpp"
#include "../utility/algorithm.hpp"
#include "../utility/functional.hpp"
#include "../utility/math.hpp"
#include "../utility/rng.hpp"
#include "../utility/utility.hpp"
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <iterator>
#include <cmath>
#include <limits>
#include <cstddef>
#include <cassert>

namespace genetic_algorithm::algorithm
{
    using RefPoint  = NSGA3::RefPoint;
    using namespace dtl;

    std::vector<RefPoint> NSGA3::generateRefPoints(size_t n, size_t dim)
    {
        assert(n > 0);
        assert(dim > 1);

        /* Generate reference point candidates randomly. */
        size_t ratio = std::max(10_sz, 2 * dim);
        std::vector<Point> candidates(ratio * n - 1);
        std::generate(candidates.begin(), candidates.end(), [&dim]() { return rng::randomSimplexPoint(dim); });

        std::vector<Point> points;
        points.reserve(n);
        points.push_back(rng::randomSimplexPoint(dim));

        std::vector<double> min_distances(candidates.size(), std::numeric_limits<double>::infinity());
        while (points.size() < n)
        {
            /* Calc the distance of each candidate to the closest ref point. */
            std::transform(GA_EXECUTION_UNSEQ, candidates.begin(), candidates.end(), min_distances.begin(), min_distances.begin(),
            [&points](const Point& candidate, double dmin) noexcept
            {
                double d = detail::euclideanDistanceSq(candidate, points.back());
                return std::min(dmin, d);
            });

            /* Add the candidate with highest min_distance to the refs. */
            size_t argmax = detail::argmax(min_distances.begin(), min_distances.end());
            points.push_back(std::move(candidates[argmax]));

            /* Remove the added candidate and the corresponding min_distance. */
            std::swap(candidates[argmax], candidates.back());
            candidates.pop_back();
            std::swap(min_distances[argmax], min_distances.back());
            min_distances.pop_back();
        }

        std::vector<RefPoint> refs;
        refs.reserve(points.size());
        std::move(points.begin(), points.end(), std::back_inserter(refs));

        return refs;
    }

    std::pair<size_t, double> NSGA3::findClosestRef(const std::vector<RefPoint>& refs, const Point& p)
    {
        assert(!refs.empty());

        auto distances = detail::map(refs, [&p](const RefPoint& ref) { return detail::perpendicularDistanceSq(ref.point, p); });
        auto idx = detail::argmin(distances.begin(), distances.end());

        return { idx, distances[idx] };
    }

    NSGA3::ASF NSGA3::getASF(std::vector<double> ideal_point, std::vector<double> weights) noexcept
    {
        assert(!weights.empty());
        assert(weights.size() == ideal_point.size());

        return [ideal_point = std::move(ideal_point), weights = std::move(weights)]
        (const std::vector<double>& fitness) noexcept
        {
            assert(fitness.size() == weights.size());

            double dmax = (ideal_point[0] - fitness[0]) / weights[0];
            for (size_t i = 0; i < fitness.size(); i++)
            {
                double d = (ideal_point[i] - fitness[i]) / weights[i];
                dmax = std::max(dmax, d);
            }

            return dmax;
        };
    }

    std::vector<double> NSGA3::weightVector(size_t dimensions, size_t axis)
    {
        assert(dimensions > axis);

        std::vector weights(dimensions, 1E-6);
        weights[axis] = 1.0;

        return weights;
    }

    void NSGA3::initialize(const GaInfo& ga)
    {
        assert(ga.num_objectives() > 1);
        assert(ga.population_size() != 0);

        auto& fitness_matrix = ga.fitness_matrix();

        ref_points_ = generateRefPoints(ga.population_size(), ga.num_objectives());

        ideal_point_ = detail::maxFitness(fitness_matrix.begin(), fitness_matrix.end());
        extreme_points_ = {};

        associatePopWithRefs(fitness_matrix.begin(), fitness_matrix.end());

        auto pfronts = nonDominatedSort(fitness_matrix.begin(), fitness_matrix.end());
        std::for_each(pfronts.begin(), pfronts.end(), [this](const FrontInfo& sol) { sol_info_[sol.idx].rank = sol.rank; });
    }

    void NSGA3::updateIdealPoint(FitnessMatrix::const_iterator first, FitnessMatrix::const_iterator last)
    {
        assert(std::distance(first, last) > 0);

        ideal_point_ = detail::max(ideal_point_, detail::maxFitness(first, last));
    }

    void NSGA3::updateExtremePoints(FitnessMatrix::const_iterator first, FitnessMatrix::const_iterator last)
    {
        assert(std::distance(first, last) > 0);
        assert(first->size() == ideal_point_.size());

        const size_t popsize = size_t(last - first);

        std::vector<Point> new_extreme_points;
        new_extreme_points.reserve(extreme_points_.size());

        for (size_t dim = 0; dim < ideal_point_.size(); dim++)
        {
            auto ASFi = getASF(ideal_point_, weightVector(ideal_point_.size(), dim));

            std::vector<double> chebysev_distances;
            chebysev_distances.reserve(extreme_points_.size() + popsize);

            std::transform(first, last, std::back_inserter(chebysev_distances), ASFi);
            std::transform(extreme_points_.begin(), extreme_points_.end(), std::back_inserter(chebysev_distances), ASFi);

            size_t idx = detail::argmin(chebysev_distances.begin(), chebysev_distances.end());
            
            (idx >= popsize) ?
                new_extreme_points.push_back(extreme_points_[idx - popsize]) :
                new_extreme_points.push_back(first[idx]);
        }

        extreme_points_ = std::move(new_extreme_points);
    }

    auto NSGA3::findNadirPoint(const std::vector<Point>& extreme_points) -> Point
    {
        assert(!extreme_points.empty());

        /* Nadir point estimate = minimum of extreme points along each objective axis. */
        Point nadir_point = extreme_points[0];
        for (size_t i = 1; i < extreme_points.size(); i++)
        {
            nadir_point = detail::min(nadir_point, extreme_points[i]);
        }

        return nadir_point;
    }

    auto NSGA3::normalize(const FitnessVector& fvec) -> FitnessVector
    {
        FitnessVector fnorm(fvec.size());

        for (size_t i = 0; i < fnorm.size(); i++)
        {
            fnorm[i] = (fvec[i] - ideal_point_[i]) / std::max(ideal_point_[i] - nadir_point_[i], 1E-6);
        }

        return fnorm;
    }

    void NSGA3::associatePopWithRefs(FitnessMatrix::const_iterator first, FitnessMatrix::const_iterator last)
    {
        assert(std::distance(first, last) > 0);
        assert(std::all_of(first, last, [first](const FitnessVector& sol) { return sol.size() == first[0].size(); }));
        assert(!ref_points_.empty());

        updateIdealPoint(first, last);
        updateExtremePoints(first, last);
        nadir_point_ = findNadirPoint(extreme_points_);
        sol_info_.resize(last - first);

        std::transform(GA_EXECUTION_UNSEQ, first, last, sol_info_.begin(), sol_info_.begin(),
        [this](const FitnessVector& f, CandidateInfo& props)
        {
            std::tie(props.ref_idx, props.ref_dist) = findClosestRef(ref_points_, normalize(f));

            return props;
        });

        std::for_each(ref_points_.begin(), ref_points_.end(), [](RefPoint& ref) { ref.niche_count = 0; });
        std::for_each(sol_info_.begin(), sol_info_.end(), [this](const CandidateInfo& info) { ref_points_[info.ref_idx].niche_count++; });
    }

    std::vector<size_t> NSGA3::nextPopulation(const GaInfo& ga,
                                              FitnessMatrix::const_iterator parents_first,
                                              FitnessMatrix::const_iterator children_first,
                                              FitnessMatrix::const_iterator children_last)
    {
        const size_t popsize = ga.population_size();

        assert(ga.num_objectives() > 1);
        assert(size_t(children_first - parents_first) == popsize);
        assert(std::all_of(parents_first, children_last, [&ga](const FitnessVector& f) { return f.size() == ga.num_objectives(); }));

        GA_UNUSED(children_first);

        auto pfronts = dtl::nonDominatedSort(parents_first, children_last);

        associatePopWithRefs(parents_first, children_last);
        std::for_each(pfronts.begin(), pfronts.end(), [this](const FrontInfo& sol) { sol_info_[sol.idx].rank = sol.rank; });
        std::for_each(ref_points_.begin(), ref_points_.end(), [](RefPoint& ref) { ref.niche_count = 0; });

        auto [partial_front_first, partial_front_last] = findPartialFront(pfronts.begin(), pfronts.end(), popsize);

        /* Keep track of the details of the candidates added to new_pop to avoid a second sort in prepareSelections(). */
        std::vector<size_t> new_pop;
        std::vector<CandidateInfo> new_info;
        new_pop.reserve(popsize);
        new_info.reserve(popsize);

        for (auto sol = pfronts.begin(); sol != partial_front_first; ++sol)
        {
            new_pop.push_back(sol->idx);
            new_info.push_back(sol_info_[sol->idx]);
            refPointOf(*sol).niche_count++;
        }

        /* Add remaining candidates from the partial front if there is one. */
        std::vector<RefPoint*> refs(partial_front_last - partial_front_first);
        std::transform(partial_front_first, partial_front_last, refs.begin(), [this](const FrontInfo& sol) { return &refPointOf(sol); });

        detail::erase_duplicates(refs);
        std::sort(refs.begin(), refs.end(), [](RefPoint* lhs, RefPoint* rhs) { return lhs->niche_count < rhs->niche_count; });

        while (new_pop.size() != ga.population_size())
        {
            /* Choose a random reference point with minimal niche count. */
            auto last_min = std::find_if(refs.begin(), refs.end(), [&refs](RefPoint* ref) { return ref->niche_count != refs[0]->niche_count; });
            RefPoint* ref = rng::randomElement(refs.begin(), last_min);

            /* Find the idx of the closest sol in the partial front associated with this ref point. */
            auto candidates = detail::find_all(partial_front_first, partial_front_last,
                                               [this, ref](const FrontInfo& sol) { return &refPointOf(sol) == ref; });

            auto& selected = *std::min_element(candidates.begin(), candidates.end(),
                                               [this](const auto& lhs, const auto& rhs) { return refDistOf(*lhs) < refDistOf(*rhs); });

            new_pop.push_back(selected->idx);
            new_info.push_back(sol_info_[selected->idx]);
            std::iter_swap(selected, --partial_front_last); /* Remove the selected candidate from the partial front so it can't be selected again. */

            /* If the selected candidate was the only one in the partial front associated with this reference point,
               the reference point needs to also be removed. */
            if (candidates.size() == 1)
            {
                refs.erase(std::find(refs.begin(), refs.end(), ref));
            }
            else /* Otherwise just increase its niche count, and make sure the refs are still sorted. */
            {
                ref->niche_count++;
                for (auto current = std::find(refs.begin(), refs.end(), ref); current != refs.end() - 1; ++current)
                {
                    if ((*current)->niche_count <= (*(current + 1))->niche_count) break;
                    std::iter_swap(current, current + 1);
                }
            }
        }
        sol_info_ = std::move(new_info);

        return new_pop;
    }

    bool NSGA3::nichedCompare(size_t lhs, size_t rhs) const noexcept
    {
        if (sol_info_[lhs].ref_idx == sol_info_[rhs].ref_idx)
        {
            if (sol_info_[lhs].rank != sol_info_[rhs].rank)
            {
                return sol_info_[lhs].rank < sol_info_[rhs].rank;
            }
            else
            {
                return sol_info_[lhs].ref_dist < sol_info_[rhs].ref_dist;
            }
        }

        return rng::randomBool();
    }

    size_t NSGA3::select(const GaInfo&, const FitnessMatrix& pop) const
    {
        assert(!pop.empty());

        size_t idx1 = rng::randomIdx(pop);
        size_t idx2 = rng::randomIdx(pop);

        return nichedCompare(idx1, idx2) ? idx1 : idx2;
    }

    std::optional<std::vector<size_t>> NSGA3::optimalSolutions(const GaInfo&) const
    {
        std::vector<size_t> optimal_indices;
        optimal_indices.reserve(sol_info_.size());

        for (size_t i = 0; i < sol_info_.size(); i++)
        {
            if (sol_info_[i].rank == 0) optimal_indices.push_back(i);
        }

        return optimal_indices;
    }

    RefPoint& NSGA3::refPointOf(const dtl::FrontInfo& sol) noexcept
    {
        return ref_points_[sol_info_[sol.idx].ref_idx];
    }
    const RefPoint& NSGA3::refPointOf(const dtl::FrontInfo& sol) const noexcept
    {
        return ref_points_[sol_info_[sol.idx].ref_idx];
    }
    double NSGA3::refDistOf(const dtl::FrontInfo& sol) const noexcept
    {
        return sol_info_[sol.idx].ref_dist;
    }

} // namespace genetic_algorithm::algorithm