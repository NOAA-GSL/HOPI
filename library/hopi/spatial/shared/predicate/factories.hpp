/**
 * @file       factories.hpp
 * @author     Bryan Flynt
 * @date       Feb 09, 2022
 * @copyright  Copyright (C) 2022 Colorado State University - All Rights Reserved
 */
#pragma once

#include "hopi/spatial/shared/predicate/distance.hpp"
#include "hopi/spatial/shared/predicate/spatial.hpp"
#include "hopi/spatial/shared/predicate/tags.hpp"

namespace hopi {
namespace spatial {
namespace shared {
namespace predicate {

/**
 * Test if disjoint from provided Bound
 *
 * Predicate will return all Bounds which do not touch
 * in any location the provided Bound.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::all_tag, detail::disjoint_tag >
Disjoint(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::all_tag, detail::disjoint_tag>(bound);
}

/**
 * Test for intersection of provided Bound
 *
 * Predicate will return all Bounds which touch the
 * provided Bound in any location.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::intersects_tag, detail::intersects_tag >
Intersects(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::intersects_tag, detail::intersects_tag>(bound);
}

/**
 * Test for overlap of provided Bound
 *
 * Predicate will return all Bounds which overlap the
 * provided Bound. Simply, intersecting the Bound is not
 * sufficient they must overlap to some extent.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::overlaps_tag, detail::overlaps_tag >
Overlaps(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::overlaps_tag, detail::overlaps_tag>(bound);
}

/**
 * Test if the provided Bound is Contained
 *
 * Predicate will return all Bounds which fully contain
 * the provided Bound. Containment can be touching or
 * extending further in each direction.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::contains_tag, detail::contains_tag >
Contains(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::contains_tag, detail::contains_tag>(bound);
}

/**
 * Test if the provided Bound is Contains others
 *
 * Predicate will return all Bounds which are contained
 * by the provided Bound. Containment can be touching or
 * extending further in each direction.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::intersects_tag, detail::contained_by_tag >
ContainedBy(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::intersects_tag, detail::contained_by_tag>(bound);
}

/**
 * Test if the provided Bound Non-Inclusively Contains others
 *
 * Predicate will return all Bounds which are contained
 * by the provided Bound. In the special case where the searched 
 * for Bounds (ie. Bounds within Tree) are points with Min==Max 
 * the points located on the maximum of the provided contains Bound
 * will not be included. This allows points to be split into distinct
 * Bounds when neighboring contains Bounds are used to search.  
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::intersects_tag, detail::contained_ni_by_tag >
ContainedByNonInclusive(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::intersects_tag, detail::contained_ni_by_tag>(bound);
}

/**
 * Test if the provided Bound is Covered
 *
 * Predicate will return all Bounds which fully cover
 * the provided Bound. Covers must extend further in
 * each direction.  Touching in any direction returns
 * a false predicate.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::covers_tag, detail::covers_tag >
Covers(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::covers_tag, detail::covers_tag>(bound);
}

/**
 * Test if the provided Bound Covers other Bounds
 *
 * Predicate will return all Bounds which are covered
 * by the provided Bound. Covers must extend further in
 * each direction.  Touching in any direction returns
 * a false predicate.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::overlaps_tag, detail::covered_by_tag >
CoveredBy(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::overlaps_tag, detail::covered_by_tag>(bound);
}

/**
 * Test if provided Bound is equal to other Bounds
 *
 * Predicate will return all Bounds which are Equal
 * in ever direction.
 */
template<typename BoundType>
spatial_predicate<BoundType, detail::intersects_tag, detail::equals_tag >
Equals(BoundType const& bound) {
	return spatial_predicate<BoundType, detail::intersects_tag, detail::equals_tag>(bound);
}



/**
 * Perform nearest neighbor search
 *
 * Predicate will return a distance predicate used for
 * performing nearest neighbor queries.
 */
template<typename BoundType>
distance_predicate<BoundType, detail::to_nearest_tag, detail::to_nearest_tag>
Nearest(BoundType const& bound, const std::size_t n) {
	return distance_predicate<BoundType, detail::to_nearest_tag, detail::to_nearest_tag>(bound,n);
}



} /* namespace predicate */
} /* namespace shared */
} /* namespace spatial */
} /* namespace hopi */


