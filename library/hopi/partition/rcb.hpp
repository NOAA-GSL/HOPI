/// @file partition.hpp
/*
 * Project:         HOPI
 * File:            partioner_rcb.hpp
 * Date:            Dec 6, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 16, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once

#include "hopi/mpixx.hpp"
#include "hopi/rtree.hpp"

#include <cmath>
#include <set>
#include <tuple>
#include <vector>

namespace hopi {

template<typename InputAdaptor>
class RCB final {
    // ----------------------------------------------------------
    // Types
    // ----------------------------------------------------------
    static constexpr auto NDim = InputAdaptor::NDim;

   public:
    using adaptor_type    = InputAdaptor;
    using size_type       = typename InputAdaptor::size_type;
    using difference_type = typename InputAdaptor::difference_type;
    using coordinate_type = typename InputAdaptor::coordinate_type;
    using rank_type       = typename InputAdaptor::rank_type;
    using weight_type     = typename InputAdaptor::weight_type;

    // ----------------------------------------------------------
    // Constructors and Operators
    // ----------------------------------------------------------
   public:
    RCB()                            = delete;
    RCB(const RCB& other)            = default;
    RCB(RCB&& other)                 = default;
    ~RCB()                           = default;
    RCB& operator=(const RCB& other) = default;
    RCB& operator=(RCB&& other)      = default;

    RCB(const mpixx::communicator& comm);

    // ----------------------------------------------------------
    // Methods
    // ----------------------------------------------------------
   public:
    void init(const adaptor_type& adapt);

    void report(const adaptor_type& adapt) const;

    // ----------------------------------------------------------
    // [PRIVATE]
    // ----------------------------------------------------------
   private:
    // Define Types
    using box_type   = hopi::spatial::BoundBox<coordinate_type, NDim>;
    using box_array  = typename box_type::array_type;
    using index_type = hopi::spatial::TreeIndex<box_type, size_type>;
    using RTree      = hopi::spatial::RTree<index_type>;

    mpixx::communicator m_comm;  ///< Communicator for everyone participating
    // std::vector<rank_type> m_ranks;  ///< Final Ranks for input points
    std::vector<box_type> m_bounds;  ///< Final Bounds for each Rank (ie. size == m_comm.size())
};

template<typename A>
RCB<A>::RCB(const mpixx::communicator& comm) : m_comm(comm)
{
}

template<typename A>
RCB<A>::~RCB()
{
}

template<typename A>
void
RCB<A>::init(const adaptor_type& adapt)
{
    // TODO: Replace Below Wrappers
    auto x    = adapt.x_;
    auto y    = adapt.y_;
    auto z    = adapt.z_;
    auto w    = adapt.w_;
    auto xinc = adapt.xinc_;
    auto yinc = adapt.yinc_;
    auto zinc = adapt.zinc_;
    auto winc = adapt.winc_;
    // TODO: Replace Above Wrappers

    // Copy the Weights or assign 1
    std::vector<weight_type> weight(local_count, 1);
    if (nullptr != w) {
        for (auto i = 0; i < local_count; ++i) {
            weight[i] = w[i * winc];
        }
    }

    // Build an RTree of Points to RCB
    RTree rtree;
    for (size_type i = 0; i < local_count; ++i) {
        box_array point = { x[i * xinc], y[i * yinc], z[i * zinc] };
        box_type  box(point, point);
        rtree.insert(index_type(box, i));
    }

    // Get Global Bounding Box for all Ranks
    std::vector<box_type> bounds_by_rank;
    const auto            my_bound = rtree.bounds();
    mpixx::all_gather(m_comm, my_bound, bounds_by_rank);

    // Determine Global Domain from each ranks Domain
    box_type global_box = bounds_by_rank[0];
    for (size_type i = 1; i < bounds_by_rank.size(); ++i) {
        global_box.stretch(bounds_by_rank[i]);
    }

    // Expand Slightly so we don't have any points on edges of domain
    global_box.next_larger();

    // Create Our Processing Arrays
    //  - boxes_to_split = Vector of {Box, NRanks} that still need to be split
    //  - final_boxes    = Ordered set of the final boxes
    //
    using box_nrank_pair = std::pair<box_type, rank_type>;
    std::vector<box_nrank_pair>                 boxes_to_split;
    std::set<box_type, typename box_type::less> final_boxes;

    // Assign how many bounds (partitions) we should build
    const size_type total_partitions = m_comm.size();
    if (total_partitions == 1) {
        final_boxes.insert(global_box);
    }
    else {
        boxes_to_split.push_back(std::make_pair(global_box, total_partitions));
    }

    //
    // While we have boxes that need to be split
    // - Find the Median for this rank
    // - Pack our median and weight to share with others
    //
    while (boxes_to_split.size() > 0) {
        std::vector<std::pair<coordinate_type, weight_type>> local_split_list;
        local_split_list.reserve(boxes_to_split.size());

        // For each Box
        // - Find the weighted median
        // - Exchange & Average
        for (auto box_index = 0; box_index < boxes_to_split.size(); ++box_index) {
            // Get handle to this Box & calc partition fractions
            const box_type& search_box      = std::get<0>(boxes_to_split[box_index]);
            const auto      total_partition = std::get<1>(boxes_to_split[box_index]);
            const auto      small_partition = rank_type(total_partition / 2);
            const auto      large_partition = rank_type(total_partition - small_partition);
            const auto      ratio_partition = double(small_partition) / double(total_partition);  // Used to split weights

            // Get all my points found within the box
            std::vector<index_type> contained_points;  // TODO: std::list might be faster
            using hopi::spatial::shared::predicate::ContainedByNonInclusive;
            rtree.query(ContainedByNonInclusive(search_box), std::back_inserter(contained_points));

            // Sort returned points along longest dimension
            const size_type long_dim = search_box.longest_dimension();
            std::sort(std::begin(contained_points), std::end(contained_points), [long_dim](const index_type& a, const index_type& b) {
                return (std::get<0>(a).center(long_dim) < std::get<0>(b).center(long_dim));
            });

            // Build a Weighted List of Points
            std::vector<weight_type> contained_weights;
            contained_weights.reserve(contained_points.size());
            for (auto i = 0; i < contained_points.size(); ++i) {
                contained_weights.push_back(weight[std::get<1>(contained_points[i])]);
            }

            // Perform a Partial Sum on the Weights & get Median
            // - Use the ratio of the NRanks split to determine where the median should be
            //
            std::partial_sum(std::begin(contained_weights), std::end(contained_weights), std::begin(contained_weights));
            auto median_iter =
                std::upper_bound(std::begin(contained_weights), std::end(contained_weights), ratio_partition * contained_weights.back());
            auto median_index = std::distance(std::begin(contained_weights), median_iter);
            auto median_value = contained_points[median_index].first.center(long_dim);

            // Pack our splits for reduction
            // const auto low_weight = contained_weights[median_index];
            // const auto hgh_weight = contained_weights.back()-contained_weights[median_index];
            const auto total_weight = contained_weights.back();
            local_split_list.emplace_back(median_value * total_weight, total_weight);
        }

        // Reduce Across All Processors
        std::vector<std::pair<coordinate_type, weight_type>> global_split_list;
        mpixx::all_reduce(m_comm, local_split_list, global_split_list, [](const auto& a, const auto& b) {
            auto ans = a;
            for (auto n = 0; n < ans.size(); ++n) {
                std::get<0>(ans[n]) = std::get<0>(b[n]);
                std::get<1>(ans[n]) = std::get<1>(b[n]);
            }
            return ans;
        });

        // For each Box
        // - Find the weighted median
        // - Split the BoundBox
        // - ReAssign Ranks to each resulting Box
        std::vector<box_nrank_pair> new_boxes_to_split;
        for (auto index = 0; index < boxes_to_split.size(); ++index) {
            // Get handle to this Box
            const box_type& search_box = std::get<0>(boxes_to_split[index]);
            const size_type long_dim   = search_box.longest_dimension();

            // Determine the weighted average split location
            const auto weighted_split = global_split_list[index].first / global_split_list[index].second;

            // Build 2 Boxes
            box_type  hgh_bound      = search_box;
            box_array new_min_corner = hgh_bound.min_corner();
            new_min_corner[long_dim] = weighted_split;
            hgh_bound.set(new_min_corner, hgh_bound.max_corner());

            box_type  low_bound      = search_box;
            box_array new_max_corner = low_bound.max_corner();
            new_max_corner[long_dim] = weighted_split;
            low_bound.set(low_bound.min_corner(), new_max_corner);

            // Split the ranks (repeat from above :>(
            const auto total_partition = std::get<1>(boxes_to_split[index]);
            const auto small_partition = rank_type(total_partition / 2);
            const auto large_partition = rank_type(total_partition - small_partition);

            if (1 == small_partition) {
                final_boxes.insert(low_bound);
            }
            else {
                new_boxes_to_split.emplace_back(low_bound, small_partition);
            }
            if (1 == large_partition) {
                final_boxes.insert(hgh_bound);
            }
            else {
                new_boxes_to_split.emplace_back(hgh_bound, large_partition);
            }
        }
        boxes_to_split = new_boxes_to_split;
    }

    // Copy the "sorted" set to the final vector
    m_bounds.clear();
    m_bounds.reserve(final_boxes.size());
    std::copy(std::begin(final_boxes), std::end(final_boxes), std::back_inserter(m_bounds));
}

template<typename A>
void
RCB<A>::report(const adaptor_type& adapt) const
{
    // TODO: Replace Below Wrappers
    auto x    = adapt.x_;
    auto y    = adapt.y_;
    auto z    = adapt.z_;
    auto w    = adapt.w_;
    auto xinc = adapt.xinc_;
    auto yinc = adapt.yinc_;
    auto zinc = adapt.zinc_;
    auto winc = adapt.winc_;
    // TODO: Replace Above Wrappers

    // Copy the Weights or assign 1
    std::vector<weight_type> weight(local_count, 1);
    if (nullptr != w) {
        for (auto i = 0; i < local_count; ++i) {
            weight[i] = w[i * winc];
        }
    }

    // Build an RTree of Points to RCB
    RTree rtree;
    for (size_type i = 0; i < local_count; ++i) {
        box_array point = { x[i * xinc], y[i * yinc], z[i * zinc] };
        box_type  box(point, point);
        rtree.insert(index_type(box, i));
    }

    // For each partition bound
    // - Find contained points
    // - Sum up total weights
    std::vector<weight_type> local_weight_total(m_bounds.size(), 0);
    for (auto i = 0; i < m_bounds.size(); ++i) {
        // Search R-Tree
        std::vector<index_type> contained_points;  // TODO: std::list might be faster
        using hopi::spatial::shared::predicate::ContainedByNonInclusive;
        rtree.query(ContainedByNonInclusive(m_bounds[i]), std::back_inserter(contained_points));

        // Sum up the Weights
        for (auto n = 0; n < contained_points.size(); ++n) {
            local_weight_total[i] += weight[contained_points[n].second];
        }
    }

    // Reduce (ie. sum) the weights across all Ranks
    std::vector<weight_type> global_weight_total;
    mpixx::all_reduce(m_comm, local_weight_total, global_weight_total, [](const auto& a, const auto& b) {
        auto ans = a;
        for (auto n = 0; n < ans.size(); ++n) {
            ans[n] += b[n];
        }
        return ans;
    });

    auto minmax_weight = std::minmax_element(global_weight_total.begin(), global_weight_total.end());
    auto sum_weight    = std::accumulate(global_weight_total.begin(), global_weight_total.end(), 0);
    auto weight_ratio  = (*minmax_weight.second - *minmax_weight.first) / sum_weight;
    auto weight_imbal  = *minmax_weight.second / *minmax_weight.first;

    if (m_comm.rank() == 0) {
        std::cout << "P:" << m_comm.rank() << "\n";
        std::cout << "    Total Bounds     = " << m_bounds.size() << "\n";
        std::cout << "    Minimum Weight   = " << *minmax_weight.first << "\n";
        std::cout << "    Maximum Weight   = " << *minmax_weight.second << "\n";
        std::cout << "    Weight Ratio     = " << weight_ratio << "\n";
        std::cout << "    Weight Imbalance = " << weight_imbal << "\n";
        std::cout << std::flush;
    }
    m_comm.barrier();
}

} /* namespace hopi */