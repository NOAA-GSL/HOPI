/// @file hopi.cpp
/*
 * Project:         HOPI
 * File:            hopi.cpp
 * Date:            Dec 6, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 12, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */

#include "hopi/mpixx.hpp"
#include "hopi/partition.hpp"

#include <cstdlib>
#include <iostream>
#include <random>

template<typename T, typename IndexedType>
void
fill_random(IndexedType& xyz, const std::pair<T, T> xyz_range = std::make_pair<T, T>(0, 1))
{
    std::random_device                rd;
    std::default_random_engine        re(rd());
    std::uniform_real_distribution<T> unif(std::get<0>(xyz_range), std::get<1>(xyz_range));
    const auto                        num_xyz = std::size(xyz);
    for (std::ptrdiff_t i = 0; i < num_xyz; ++i) {
        xyz[i] = unif(re);
    }
}

template<typename IndexedType>
void
duplicate_vector(const std::size_t ndup, const std::size_t ndim, IndexedType& xyz)
{
    const auto orig_size  = std::size(xyz);
    const auto num_points = orig_size / ndim;
    xyz.resize(orig_size + ndup * ndim * num_points);
    for (std::ptrdiff_t r = 1; r < (ndup + 1); ++r) {
        for (std::ptrdiff_t i = 0; i < num_points; ++i) {
            for (std::ptrdiff_t d = 0; d < ndim; ++d) {
                xyz[orig_size + (i * ndim + d)] = xyz[i * ndim + d];
            }
        }
    }
}

struct UserTypes {
    static constexpr std::size_t NDim = 3;

    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using coordinate_type = double;
    using rank_type       = int;
    using weight_type     = double;
};



int
main(int argc, char* argv[])
{
    // Start MPI
    mpixx::environment  env;
    mpixx::communicator world;
    auto                my_rank   = world.rank();
    auto                num_ranks = world.size();

    // ============================================================
    //                   Bogus Data (Testing Only)
    // ============================================================

    constexpr std::size_t ND    = UserTypes::NDim;  // # of Dimensions
    //constexpr std::size_t Nt    = 1000;  // # of Target Points
    constexpr std::size_t Ns    = 1000;  // # of Source Points
    constexpr std::size_t Nc    = 50;    // # of Points in Cloud
    constexpr std::size_t Ntdup = 3;     // # of Target Duplicates
    constexpr std::size_t Nsdup = 5;     // # of Source Duplicates

    const std::size_t Nt = 10000 / num_ranks;

    // Init Data
    std::vector<UserTypes::coordinate_type> target_xyz(Nt * ND);
    std::vector<UserTypes::coordinate_type> source_xyz(Ns * ND);

    // Fill Random Targets & Sources
    fill_random<UserTypes::coordinate_type>(target_xyz, { -100, 100 });
    fill_random<UserTypes::coordinate_type>(source_xyz, { -100, 100 });

    // Insert duplicates into the data (simulates GeoFLOW)
    // duplicate_vector(Ntdup, ND, target_xyz);
    // duplicate_vector(Nsdup, ND, source_xyz);

    // ============================================================
    //                 End Bogus Data (Testing Only)
    // ============================================================

    // ----------------------------------------------------------
    // Remove Duplicates
    // ----------------------------------------------------------

    // ----------------------------------------------------------
    // Create Search Data Structure for Targets
    // ----------------------------------------------------------

    hopi::Partition<UserTypes> partition(world);
    partition.init(Nt, target_xyz.data(), ND, target_xyz.data() + 1, ND, target_xyz.data() + 2, ND, nullptr, 1);

    partition.report(Nt, target_xyz.data(), ND, target_xyz.data() + 1, ND, target_xyz.data() + 2, ND, nullptr, 1);

    std::cout << "P:" << my_rank << " -- DONE-- " << std::endl;
    return EXIT_SUCCESS;
}