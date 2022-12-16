/**
 * @file       dispatch.hpp
 * @author     Bryan Flynt
 * @date       Feb 09, 2022
 * @copyright  Copyright (C) 2022 Colorado State University - All Rights Reserved
 */
#pragma once



#include "hopi/spatial/bound/box.hpp"
#include "hopi/spatial/shared/predicate/tags.hpp"

namespace hopi {
namespace spatial {
namespace shared {
namespace predicate {
namespace detail {

//-------------------------------------------------------------------------
// Default should never be called
//-------------------------------------------------------------------------

template<typename PredTag>
struct dispatch{
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b);
};

//-------------------------------------------------------------------------
// Spatial Proximity Type Dispatches
//-------------------------------------------------------------------------

template<>
struct dispatch<detail::disjoint_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Disjoint(a,b);
	}
};

template<>
struct dispatch<detail::intersects_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Intersects(a,b);
	}
};

template<>
struct dispatch<detail::overlaps_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Overlaps(a,b);
	}
};

template<>
struct dispatch<detail::contains_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Contains(a,b);
	}
};

template<>
struct dispatch<detail::contained_by_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Contains(b,a);
	}
};

template<>
struct dispatch<detail::contained_ni_by_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::ContainsNonInclusive(b,a);
	}
};

template<>
struct dispatch<detail::covers_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Covers(a,b);
	}
};

template<>
struct dispatch<detail::covered_by_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Covers(b,a);
	}
};

template<>
struct dispatch<detail::equals_tag> {
	template<typename BoundType>
	static bool apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Equals(a,b);
	}
};

template<>
struct dispatch<detail::all_tag> {
	template<typename BoundType>
	static constexpr bool apply(const BoundType& a, const BoundType& b) noexcept {
		return true;
	}
};



//-------------------------------------------------------------------------
// Distance Type Dispatches
//-------------------------------------------------------------------------

template<>
struct dispatch<detail::to_nearest_tag> {
	template<typename BoundType>
	static typename BoundType::value_type
	apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Nearest(a,b);
	}
};

template<>
struct dispatch<detail::to_centroid_tag> {
	template<typename BoundType>
	static typename BoundType::value_type
	apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Centroid(a,b);
	}
};

template<>
struct dispatch<detail::to_furthest_tag> {
	template<typename BoundType>
	static typename BoundType::value_type
	apply(const BoundType& a, const BoundType& b) noexcept {
		return hopi::spatial::bound::Furthest(a,b);
	}
};

} /* namespace detail */
} /* namespace predicate */
} /* namespace shared */
} /* namespace spatial */
} /* namespace hopi */



