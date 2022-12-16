/// @file all.hpp
/*
 * Project:         HOPI
 * File:            all.hpp
 * Date:            Dec 6, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 6, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once


#include "hopi/spatial/bound/box.hpp"
#include "hopi/spatial/common/truncated_multiset.hpp"
#include "hopi/spatial/shared/index/rtree/algorithm.hpp"
#include "hopi/spatial/shared/index/rtree/leaf.hpp"
#include "hopi/spatial/shared/index/rtree/linear.hpp"
#include "hopi/spatial/shared/index/rtree/node.hpp"
#include "hopi/spatial/shared/index/rtree/page.hpp"
#include "hopi/spatial/shared/index/rtree/quadratic.hpp"
#include "hopi/spatial/shared/index/exhaustive.hpp"
#include "hopi/spatial/shared/index/rtree.hpp"
#include "hopi/spatial/shared/predicate/dispatch.hpp"
#include "hopi/spatial/shared/predicate/distance.hpp"
#include "hopi/spatial/shared/predicate/factories.hpp"
#include "hopi/spatial/shared/predicate/spatial.hpp"
#include "hopi/spatial/shared/predicate/tags.hpp"
