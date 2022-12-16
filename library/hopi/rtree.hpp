/// @file rtree.hpp
/*
 * Project:         HOPI
 * File:            rtree.hpp
 * Date:            Dec 6, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 6, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once

#include "hopi/spatial/all.hpp"

#include <utility> // std::pair

namespace hopi {
namespace spatial {

namespace detail_extractor {
template<typename PairType>
struct pair_extractor {
	using bound_type = typename PairType::first_type;
	using key_type   = typename PairType::second_type;

	const bound_type& operator()(PairType const& pair) const {
		return pair.first;
	}
};
template<typename TupleType>
struct tuple_extractor {
	using bound_type = typename std::tuple_element<0,TupleType>::type;
	const bound_type& operator()(TupleType const& tup) const {
		return std::get<0>(tup);
	}
};
} 

//
// Bounding Box Type
//
template<typename T, std::size_t N>
using BoundBox = ::hopi::spatial::bound::Box<T,N>;

//
// Point Types Used
//
template<typename T, std::size_t N>
using Point = typename BoundBox<T,N>::array_type;

//
// R-Tree Key that is stored
//
template<typename BoxType, typename KeyType>
using TreeIndex = std::pair<BoxType, KeyType>;

//
// Extraction Functor to Get Bound from Index
//
template<typename IndexType>
using IndexExtractor = detail_extractor::pair_extractor<IndexType>;

//
// R-Tree Type
//
template<typename IndexType>
using RTree = shared::index::RTree<IndexType, IndexExtractor<IndexType>, shared::index::rtree::Quadratic<10,4>>;


} // namespace spatial
} // namespace hopi