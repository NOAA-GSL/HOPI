/// @file mpixx.hpp
/*
 * Project:         HOPI
 * File:            mpixx.hpp
 * Date:            Dec 6, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 8, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once

#include "boost/mpi.hpp"

// To enable Boost Serialization for MPI
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

namespace mpixx = boost::mpi;

namespace hopi {

void mpixx_abort_handler();

} // namespace hopi

//
// Enable serialization of std::tuple
//
template <typename Archive, typename... Types>
void boost::serialization::serialize(Archive &ar, std::tuple<Types...> &t, const unsigned int)
{
    std::apply([&](auto &...element)
                { ((ar & element), ...); },
                t);
}
