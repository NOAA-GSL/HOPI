/// @file ascii_targets.hpp
/*
 * Project:         HOPI
 * File:            ascii_targets.hpp
 * Date:            Sep 30, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Sep 30, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once

#include <cstdlib>
#include <string>
#include <vector>

namespace hopi {

/// Read Target ASCII File
/**
 */
void read_target_file(const std::string& file_name, std::size_t& ndim, std::size_t& npoints, std::vector<double>& xyz);

/// Write Target ASCII File
/**
 */
void write_target_file(const std::string&         file_name,
                       const std::size_t&         ndim,
                       const std::size_t&         npoints,
                       const std::vector<double>& xyz,
                       const std::size_t&         nvar,
                       const std::vector<double>& var);

} /* namespace hopi */