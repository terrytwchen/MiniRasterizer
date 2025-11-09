#pragma once
#include <cmath>

struct Vec3
{
public:
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0)
        : x(x), y(y), z(z)
    {
    }

    Vec3 operator*(const float& other) const
    {
        return Vec3(x * other, y * other, z * other);
    }

    Vec3 operator*(const Vec3& other) const
    {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    Vec3 operator+(const Vec3& other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    float dot(const Vec3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 normalize() const
    {
        float len = std::sqrt(x * x + y * y + z * z);
        if (len == 0)
            return Vec3(0, 0, 0);
        return Vec3(x / len, y / len, z / len);
    }
};


// Non-member operator overload to support scalar multiplication from the left (e.g., 5.0f * vec).
// Defined as inline in the header to prevent One Definition Rule (ODR) linker errors.
inline Vec3 operator*(const float& other, const Vec3& vec)
{
    return vec * other;
}