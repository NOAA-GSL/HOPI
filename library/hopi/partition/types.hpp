/// @file types.hpp
/*
 * Project:         HOPI
 * File:            types.hpp
 * Date:            Dec 16, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 16, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once


#include <cstdlib>
#include <type_traits>

namespace hopi {
namespace partition {

template<typename ValueType, typename SizeType = std::size_t, typename WeightType = ValueType>
struct Types {
    using size_type       = SizeType;
    using difference_type = std::make_signed<SizeType>::type;
    using coordinate_type = ValueType;
    using weight_type     = WeightType
};

}  // namespace partition
}  // namespace hopi
