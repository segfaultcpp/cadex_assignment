#pragma once

#include <string>
#include <format>
#include <cstdint>
#include <compare>
#include <string_view>

#define DEFAULT_COPY_MOVE(T) \
    T(const T&) = default; \
    T& operator=(const T&) = default; \
    T(T&&) noexcept = default; \
    T& operator=(T&&) noexcept = default;

namespace cadex {
#ifdef USE_F64
    using FloatType = double;
#else
    using FloatType = float;
#endif
    
    struct Vec3 {
        FloatType X = 0.f;
        FloatType Y = 0.f;
        FloatType Z = 0.f;

        constexpr explicit Vec3() = default;

        constexpr Vec3(FloatType x, FloatType y, FloatType z)
            : X(x)
            , Y(y)
            , Z(z)
        {}

        constexpr auto operator<=>(const Vec3& other) const = default;
    };
    
    class ACurve {
        std::string mName;
        uint64_t mId;
        Vec3 mOrigin;

    public:
        ACurve(std::string&& name, uint64_t id, Vec3 origin)
            : mName(std::move(name))
            , mId(id)
            , mOrigin(origin)
        {}
        virtual ~ACurve() = default;

        DEFAULT_COPY_MOVE(ACurve);

        std::string_view Name() const {
            return mName;
        }

        uint64_t Id() const {
            return mId;
        }

        Vec3 Origin() const {
            return mOrigin;
        }

        virtual Vec3 Point(FloatType t) = 0;
        virtual Vec3 Der(FloatType t) = 0;
    };
    
    struct CircleData {
        FloatType Radius = 0.f;
    };
    
    struct EllipseData {
        FloatType MinorRadius = 0.f;
        FloatType MajorRadius = 0.f;
    };
    
    struct HelixData {
        FloatType Radius = 0.f;
        FloatType Step = 0.f;
    };
    
    template<typename CurveData>
    class Curve final : public ACurve {
        CurveData mData;

    public:
        Curve(std::string&& name, uint64_t id, Vec3 origin, CurveData data)
            : ACurve(std::move(name), id, origin)
            , mData(data)
        {}
        ~Curve() override = default;

        DEFAULT_COPY_MOVE(Curve);

        CurveData Data() const {
            return mData;
        }

        Vec3 Point(FloatType t) override;
        Vec3 Der(FloatType t) override;
    };

    using Circle = Curve<CircleData>;
    using Ellipse = Curve<EllipseData>;
    using Helix = Curve<HelixData>;
}

template<>
struct std::formatter<cadex::Vec3> : std::formatter<std::string_view> {
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        return std::formatter<std::string_view>::parse(ctx);
    }
 
    template<class FmtContext>
    FmtContext::iterator format(cadex::Vec3 v, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "Vec3{{ {}, {}, {} }}", v.X, v.Y, v.Z);
    } 
};
