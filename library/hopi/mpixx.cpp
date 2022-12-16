/// @file mpixx.cpp
/*
 * Project:         HOPI
 * File:            mpixx.cpp
 * Date:            Dec 6, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 6, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */

#include "hopi/mpixx.hpp"

#include <cstdlib> // exit()

namespace hopi {

void mpixx_abort_handler(){
	mpixx::communicator world;
	world.abort(EXIT_FAILURE);
}

} // namespace hopi