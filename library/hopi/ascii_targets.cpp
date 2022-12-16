/// @file ascii_targets.cpp
/*
 * Project:         HOPI
 * File:            ascii_targets.hpp
 * Date:            Sep 30, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 6, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */

#include "hopi/ascii_targets.hpp"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace hopi {

/// Read Target ASCII File
/**
 */
void
read_target_file(const std::string& file_name, std::size_t& ndim, std::size_t& npoints, std::vector<double>& xyz)
{
    std::ifstream file(file_name);
    if (not file) {
        std::cerr << "ERROR: File Did Not Open" << std::endl;
        std::cerr << "Filename: " << file_name << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Read whole file into buffer
    std::stringstream file_stream;
    file_stream << file.rdbuf();
    file.close();

    // Parse Header
    file_stream >> ndim;
    if (ndim > 3) {
        std::cerr << "ERROR: Wrong Number of Dimensions In File" << std::endl;
        std::cerr << "Number of Dimensions = " << ndim << std::endl;
        std::exit(EXIT_FAILURE);
    }
    file_stream >> npoints;

    // Parse Data
    xyz.resize(ndim * npoints);
    for (std::size_t i = 0; i < npoints; ++i) {
        for (std::size_t j = 0; j < ndim; ++j) {
            file_stream >> xyz[i * ndim + j];
        }
    }
}

/// Write Target ASCII File
/**
 */
void
write_target_file(const std::string&         file_name,
                  const std::size_t&         ndim,
                  const std::size_t&         npoints,
                  const std::vector<double>& xyz,
                  const std::size_t&         nvar,
                  const std::vector<double>& var)
{
    assert(npoints == xyz.size() / ndim);
    assert(npoints == var.size() / nvar);

    std::ofstream file(file_name);
    if (not file) {
        std::cerr << "ERROR: File Did Not Open" << std::endl;
        std::cerr << "Filename: " << file_name << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Write Header
    file << std::setw(10) << ndim;
    file << std::setw(10) << npoints;
    file << std::setw(10) << nvar;
    file << std::endl;

    // Write Data
    for (std::size_t i = 0; i < npoints; ++i) {
        for (std::size_t j = 0; j < ndim; ++j) {
            file << std::setw(15) << std::setprecision(8) << std::scientific << xyz[i * ndim + j];
        }
        for (std::size_t j = 0; j < nvar; ++j) {
            file << std::setw(15) << std::setprecision(8) << std::scientific << var[i * ndim + j];
        }
        file << std::endl;
    }

    // Close file
    file.close();
}

} /* namespace hopi */