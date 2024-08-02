#include "Types.h"
#include "ParticleUtils.h"

namespace ParticleUtils{
    // Conversion functions
    float halfToFloat(half h)
    {
        uint32_t sign = (h >> 15) & 0x1;
        uint32_t exponent = (h >> 10) & 0x1f;
        uint32_t mantissa = h & 0x3ff;

        if (exponent == 0)
        {
            if (mantissa == 0)
            {
                return sign ? -0.0f : 0.0f;
            }
            else
            {
                float result = std::ldexp(static_cast<float>(mantissa), -24);
                return sign ? -result : result;
            }
        }
        else if (exponent == 31)
        {
            if (mantissa == 0)
            {
                return sign ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
            }
            else
            {
                return std::numeric_limits<float>::quiet_NaN();
            }
        }

        float result = std::ldexp(static_cast<float>((mantissa | 0x400)), static_cast<int>(exponent) - 25);
        return sign ? -result : result;
    }

    half floatToHalf(float f)
    {
        uint32_t x = *reinterpret_cast<uint32_t*>(&f);
        uint32_t sign = (x >> 31) & 0x1;
        uint32_t exponent = (x >> 23) & 0xff;
        uint32_t mantissa = x & 0x7fffff;

        if (exponent == 0)
        {
            return static_cast<half>(sign << 15);
        }
        else if (exponent == 0xff)
        {
            if (mantissa == 0)
            {
                return static_cast<half>((sign << 15) | 0x7c00);
            }
            else
            {
                return static_cast<half>((sign << 15) | 0x7c00 | (mantissa >> 13));
            }
        }

        int newExponent = exponent - 127 + 15;
        if (newExponent >= 31)
        {
            return static_cast<half>((sign << 15) | 0x7c00);
        }
        else if (newExponent <= 0)
        {
            return static_cast<half>(sign << 15);
        }

        return static_cast<half>((sign << 15) | (newExponent << 10) | (mantissa >> 13));
    }
};