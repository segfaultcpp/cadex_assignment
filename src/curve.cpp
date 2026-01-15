#include "curve.hpp"

#include <cmath>
#include <numbers>

namespace cadex {
    template<>
    Vec3 Circle::Point(FloatType t) {
        const Vec3 origin = Origin();
        return Vec3 {
            origin.X + mData.Radius * std::cos(t),
            origin.Y + mData.Radius * std::sin(t),
            origin.Z
        };
    }
    
    template<>
    Vec3 Circle::Der(FloatType t) {
        return Vec3 {
            -mData.Radius * std::sin(t),
            mData.Radius * std::cos(t),
            0.0
        };
    }
    
    template<>
    Vec3 Ellipse::Point(FloatType t) {
        const Vec3 origin = Origin();
        return Vec3 {
            origin.X + mData.MajorRadius * std::cos(t),
            origin.Y + mData.MinorRadius * std::sin(t),
            origin.Z
        };
    }
    
    template<>
    Vec3 Ellipse::Der(FloatType t) {
        return Vec3 {
            -mData.MajorRadius * std::sin(t),
            mData.MinorRadius * std::cos(t),
            0.0
        };
    }
    
    template<>
    Vec3 Helix::Point(FloatType t) {
        constexpr auto PI = std::numbers::pi_v<FloatType>;
        const Vec3 origin = Origin();
        return Vec3 {
            origin.X + mData.Radius * std::cos(t),
            origin.Y + mData.Radius * std::sin(t),
            origin.Z + mData.Step / (FloatType(2.0) * PI) * t
        };
    }
    
    template<>
    Vec3 Helix::Der(FloatType t) {
        constexpr auto PI = std::numbers::pi_v<FloatType>;
        return Vec3 {
            -mData.Radius * std::sin(t),
            mData.Radius * std::cos(t),
            mData.Step / (FloatType(2.0) * PI)
        };
    }
}
