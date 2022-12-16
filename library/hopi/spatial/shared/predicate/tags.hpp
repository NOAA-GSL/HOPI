/**
 * @file       tags.hpp
 * @author     Bryan Flynt
 * @date       Feb 09, 2022
 * @copyright  Copyright (C) 2022 Colorado State University - All Rights Reserved
 */
#pragma once


namespace hopi {
namespace spatial {
namespace shared {
namespace predicate {
namespace detail {

/**
 * Tag to indicate operation to perform
 */
struct disjoint_tag final {};
struct intersects_tag final {};
struct overlaps_tag final {};
struct contains_tag final {};
struct contained_by_tag final {};
struct contained_ni_by_tag final {};
struct covers_tag final {};
struct covered_by_tag final {};
struct equals_tag final {};
struct all_tag final {};

/**
 * Tag to indicate distance operation to perform
 */
struct to_nearest_tag final {};
struct to_centroid_tag final {};
struct to_furthest_tag final {};



} /* namespace detail */
} /* namespace predicate */
} /* namespace shared */
} /* namespace spatial */
} /* namespace hopi */


