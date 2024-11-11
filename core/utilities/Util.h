#pragma once

#include "../constants.h"

#include <string>
#include <algorithm>
#include <cmath>


// A small set of utility functions
namespace util {
    inline double degrees_to_radians(const double degrees) noexcept {
        return degrees * M_PI / static_cast<double>(180);
    }

    inline double clamp(const double x, const double lower, const double upper) noexcept {
        #ifdef USE_BOOK_CLAMP
            if (x < lower)
                return lower;
            if (x > upper)
                return upper;

            return x;
        #else
            // C++17 and above has a built-in clamp function.   if not, it can also be recreated with `min()` and `max()` calls
            // After inspecting on godbolt, using std::clamp save about one instruction.  But after measuring
            // the effects seemed almost negligable.
            return std::clamp(x, lower, upper);
        #endif
    }

    // Note, this is actually an approximation by Christophine Schlick for determine when a material
    // becomes mirrorlike at an angle
    inline double schlick(const double cosine, const double refractive_index) noexcept {
        #ifdef USE_BOOK_SCHLICK
            rreal r0 = (1 - refractive_index) / (1 + refractive_index);
            r0 = r0 * r0;
            return r0 + ((1 - r0) * std::pow((1 - cosine), 5));
        #else
            // This tries to take advantage of the compiler converting these math operations
            // into vectorized instructions.  std::pow() can also be kinda slow compared to
            // writing out all of the multiplies.
            //    https://baptiste-wicht.com/posts/2017/09/cpp11-performance-tip-when-to-use-std-pow.html
            const double alpha = 1 - cosine;
            const double beta = alpha * alpha * alpha * alpha * alpha;

            const double r0 = (1 - refractive_index) / (1 + refractive_index);
            const double r1 = r0 * r0;

            return r1 + ((1 - r1) * beta);
        #endif
    }

    #ifndef USE_BOOK_SIN_COS
    double _cos_approx_private(double x) noexcept;
    #endif

    inline double cos(const double x) noexcept {
        #ifdef USE_BOOK_SIN_COS
            return std::cos(x);
        #else
            return _cos_approx_private(x);
        #endif
    }

    inline double sin(const double x) noexcept {
        #ifdef USE_BOOK_SIN_COS
            return std::sin(x);
        #else
            // Due to trig identities, we can reuse our cosine computation (with an offset)
            return util::cos(M_PI_2 - x);
        #endif
    }


    #ifndef USE_BOOK_ATAN2
    double _atan2_approx_private(const double y, const double x) noexcept;
    #endif

    inline double atan2(const double y, const double x) noexcept {
        #ifdef USE_BOOK_ATAN2
            return std::atan2(y, x);
        #else
            return _atan2_approx_private(y, x);
        #endif
    }



    #ifndef USE_BOOK_ASIN
        double _asin_approx_private(const double x) noexcept;
    #endif

    inline double asin(const double x) noexcept {
        #ifdef USE_BOOK_ASIN
            return std::asin(x);
        #else
            return _asin_approx_private(x);
        #endif
    }


    #ifndef USE_BOOK_SQRT
        double _sqrt_approx_private(const double x) noexcept;
    #endif

    inline double sqrt(const double x) noexcept {
        #ifdef USE_BOOK_SQRT
            return std::sqrt(x);
        #else
            return _sqrt_approx_private(x);
        #endif
    }
}
