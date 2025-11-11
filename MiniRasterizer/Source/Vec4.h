/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include <cmath>

struct Vec4
{
public:
    float x, y, z, w;
    Vec4(float x = 0, float y = 0, float z = 0, float w = 0)
        : x(x), y(y), z(z), w(w)
    {
    }

    Vec4 operator*(const float& other) const
    {
        return Vec4(x * other, y * other, z * other, w * other);
    }

    Vec4 operator+(const Vec4& other) const
    {
        return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Vec4 operator-(const Vec4& other) const
    {
        return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    float dot(const Vec4& other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    Vec4 normalize() const
    {
        float len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len == 0)
            return Vec4(0, 0, 0, 0);
        return Vec4(x / len, y / len, z / len, w / len);
    }
};


// Non-member operator overload to support scalar multiplication from the left (e.g., 5.0f * vec).
// Defined as inline in the header to prevent One Definition Rule (ODR) linker errors.
inline Vec4 operator*(const float& other, const Vec4& vec)
{
    return vec * other;
}
