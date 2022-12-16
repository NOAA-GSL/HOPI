/// @file leaf.cpp
/*
 * Project:         HOPI
 * File:            leaf.hpp
 * Date:            Sep 30, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 6, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once

namespace hopi {
namespace spatial {
namespace shared {
namespace index {
namespace rtree {


template<typename Value, typename BoundExtractor>
class Leaf {

	//-------------------------------------------------------------------------
	// Types & Constants
	//-------------------------------------------------------------------------
private:
	using self_type = Leaf<Value,BoundExtractor>;

public:
	using value_type       = Value;
	using bound_extractor  = BoundExtractor;
	using bound_type       = typename bound_extractor::bound_type;
	using bound_value_type = typename bound_type::value_type;


	//-------------------------------------------------------------------------
	// Constructors
	//-------------------------------------------------------------------------

	Leaf() = default;

	Leaf(const Leaf& other) = default;

	Leaf(Leaf&& other) = default;

	~Leaf() = default;

	Leaf(value_type const& value) : value_(value) {
	}

	//-------------------------------------------------------------------------
	// Assignment Operators
	//-------------------------------------------------------------------------

	Leaf& operator=(const Leaf& box) = default;

	Leaf& operator=(Leaf&& box) = default;

	//-------------------------------------------------------------------------
	// Modifiers
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Element Access
	//-------------------------------------------------------------------------

	value_type const& getValue() const noexcept {
		return value_;
	}

	//-------------------------------------------------------------------------
	// Iterators
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	// Capacity
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	// Indexing
	//-------------------------------------------------------------------------

	const bound_type& getBound() const noexcept {
		static const bound_extractor extract;
		return extract(value_);
	}

	//-------------------------------------------------------------------------
	// Data [Private]
	//-------------------------------------------------------------------------
private:
	value_type value_;

	//-------------------------------------------------------------------------
	// Internal Functions [Private]
	//-------------------------------------------------------------------------
private:

};




} /* namespace rtree */
} /* namespace index */
} /* namespace shared */
} /* namespace spatial */
} /* namespace hopi */



