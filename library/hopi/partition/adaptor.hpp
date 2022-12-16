/// @file adaptor.hpp
/*
 * Project:         HOPI
 * File:            adaptor.hpp
 * Date:            Dec 16, 2022
 * Author:          Bryan Flynt
 * -----
 * Last Modified:   Dec 16, 2022
 * Modified By:     Bryan Flynt
 * -----
 * Copyright:       See LICENSE file
 */
#pragma once

namespace hopi {
namespace partition {

template<typename DataTypes>
class WeightedAdaptor3D final {

    // ============================================================
    //                      Types & Constants
    // ============================================================
   public:
    using size_type       = typename DataTypes::size_type;
    using difference_type = typename DataTypes::difference_type;
    using coordinate_type = typename DataTypes::coordinate_type;
    using weight_type     = typename DataTypes::weight_type;
    using rank_type       = int;

    static constexpr size_type NDim = 3;

    // ============================================================
    //                        Constructors
    // ============================================================
   public:
    WeightedAdaptor3D()                               = delete;
    WeightedAdaptor3D(const WeightedAdaptor3D& other) = default;
    WeightedAdaptor3D(WeightedAdaptor3D&& other)      = default;
    ~WeightedAdaptor3D()                              = default;

    WeightedAdaptor3D(const size_type        count,
                      const coordinate_type* x,
                      const difference_type  xinc,
                      const coordinate_type* y,
                      const difference_type  yinc,
                      const coordinate_type* z,
                      const difference_type  zinc,
                      const weight_type*     w,
                      const difference_type  winc);

    // ============================================================
    //                         Operators
    // ============================================================
   public:
    WeightedAdaptor3D& operator=(const WeightedAdaptor3D& other) = default;
    WeightedAdaptor3D& operator=(WeightedAdaptor3D&& other)      = default;



    // ============================================================
    //                         [PRIVATE]
    // ============================================================
   private:

};

}  // namespace partition
}  // namespace hopi
