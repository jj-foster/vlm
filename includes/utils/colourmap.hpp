#pragma once

#include <algorithm>
#include <cmath>

namespace utils
{
    namespace colourMap
    {
        enum class CmType
        {
            Viridis, Jet, Coolwarm
        };

        struct Colour
        {
            double data[3];

            constexpr Colour(double r, double g, double b) noexcept : data{ r,g,b } {}

            double& r() noexcept { return data[0]; }
            double& g() noexcept { return data[1]; }
            double& b() noexcept { return data[2]; }
            constexpr double r() const noexcept { return data[0]; }
            constexpr double g() const noexcept { return data[1]; }
            constexpr double b() const noexcept { return data[2]; }

            // Multiplier operator overload - multiplies each rgb element by a value
            friend constexpr Colour operator*(double s, const Colour& c) noexcept
            {
                return { s * c.r(),s * c.g(),s * c.b() };
            }

            // + operator overload - Adds rbg values of each colour together
            friend constexpr Colour operator+(const Colour& c0, const Colour& c1) noexcept
            {
                return { c0.r() + c1.r(),c0.g() + c1.g(),c0.b() + c1.g() };
            }

        };

        inline Colour getColour(double x, CmType type = CmType::Viridis);
        inline Colour GetViridisColour(double x);
        inline Colour GetJetColour(double x);
        inline Colour GetCoolwarmColour(double x);

        // Not for external use
        namespace internal
        {
            inline constexpr double Clamp01(double x) noexcept
            {
                return (x < 0.0) ? 0.0 : (x > 1.0) ? 1.0 : x;
            }

            template <std::size_t N>
            Colour CalcLerp(double x, const Colour(&data)[N])
            {
                const double a = Clamp01(x) * (N - 1);
                const double i = std::floor(a);
                const double t = a - i;
                const Colour& c0 = data[static_cast<std::size_t>(i)];
                const Colour& c1 = data[static_cast<std::size_t>(std::ceil(a))];

                return (1.0 - t) * c0 + t * c1;
            }
        }

        inline Colour getColour(double x, CmType type)
        {
            switch (type)
            {
            case CmType::Viridis:
                return GetViridisColour(x);
            case CmType::Jet:
                return GetJetColour(x);
            case CmType::Coolwarm:
                return GetCoolwarmColour(x);
            default:
                break;
            }
        }

        inline Colour GetViridisColour(double x)
        {
            // RBG values (0 to 1)
            constexpr Colour data[]{
                {0.267003985,	0.004872566,	0.329415069},
                {0.274741032,	0.196973267,	0.497250443},
                {0.212671237,	0.359101377,	0.551635047},
                {0.152958099,	0.498051451,	0.557685327},
                {0.122053592,	0.632105543,	0.530848866},
                {0.290013937,	0.758845119,	0.427827161},
                {0.622182341,	0.853814293,	0.226247911},
                {0.993248149,	0.906154763,	0.143935944}

            };

            return internal::CalcLerp(x, data);
        }

        inline Colour GetJetColour(double x)
        {
            constexpr Colour data[]
            {
                { 0.0, 0.0, 0.5 },
                { 0.0, 0.0, 1.0 },
                { 0.0, 0.5, 1.0 },
                { 0.0, 1.0, 1.0 },
                { 0.5, 1.0, 0.5 },
                { 1.0, 1.0, 0.0 },
                { 1.0, 0.5, 0.0 },
                { 1.0, 0.0, 0.0 },
                { 0.5, 0.0, 0.0 }
            };

            return internal::CalcLerp(x, data);
        }

        inline Colour GetCoolwarmColour(double x)
        {
            constexpr Colour data[]
            {
                {0.229999504,	0.298998934,	0.754000139},
                {0.406839976,	0.537716815,	0.934353077},
                {0.602704657,	0.731255644,	0.999993038},
                {0.78841419,	0.845877766,	0.939423093},
                {0.930635713,	0.820337799,	0.761004578},
                {0.967788492,	0.657029313,	0.537326447},
                {0.88710666,    0.413948424,	0.324564482},
                {0.706000136,	0.015991824,	0.150000072}
            };

            return internal::CalcLerp(x, data);
        }
    }
}