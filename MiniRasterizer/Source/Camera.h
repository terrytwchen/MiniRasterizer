#pragma once
#include "Vec3.h"

struct Camera
{
    Vec3 position;
    Vec3 direction;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera(Vec3 pos = Vec3(0.0f, 0.0f, 10.0f),
        Vec3 dir = Vec3(0.0f, 0.0f, -1.0f),
        float fov = 60.0f,
        float aspect = 1080.0f / 720.0f,
        float near = 0.1f,
        float far = 100.0f)
        : position(pos),
        direction(dir.normalize()),
        fov(fov * 3.14159265f / 180.0f),
        aspectRatio(aspect),
        nearPlane(near),
        farPlane(far)
    {
    }
};