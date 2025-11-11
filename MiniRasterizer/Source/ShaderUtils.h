/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "Camera.h"
#include <cmath>
#include <algorithm>

namespace ShaderUtils
{
    // Model-to-World transform
    inline Vec3 TransformModelToWorld(const Vec3& positionMS, const Vec3& objectPosition)
    {
        // [SIMPLIFICATION]
        // This is a simplified Model-to-World transform that only handles translation.
        // It does not account for object rotation or scale.
        //
        // A full implementation would use a 4x4 Model Matrix (Mat4x4):
        // Mat4x4 modelMatrix = object.GetModelMatrix(); // (from pos, rot, scale)
        // Vec4 worldPos = modelMatrix * Vec4(positionMS, 1.0f);
        return positionMS + objectPosition;
    }
    
    // Model-to-View transform
    inline Vec3 TransformWorldToView(const Vec3& positionWS, const Camera& camera)
    {
        // [SIMPLIFICATION]
        // This is a simplified World-to-View transform (view = world - cameraPos).
        // It only handles a camera's translation and assumes no rotation.
        //
        // A full implementation would use a 4x4 View Matrix (Mat4x4) generated from camera position AND rotation (e.g., a "lookAt" matrix):
        // Mat4x4 viewMatrix = camera.GetViewMatrix();
        // Vec4 viewPos = viewMatrix * Vec4(positionWS, 1.0f);
        return positionWS - camera.position;
    }

    // View-to-Clip (Projection) transform
    inline Vec4 TransformViewToClip(const Vec3& positionVS, const Camera& camera)
    {
        // [CRITICAL STEP]
        // This function performs the Perspective Projection, converting View Space (Vec3) into Clip Space (Vec4).
        //
        // The 'w' component is generated here (w = -viewPos.z). This 'w' is the "magic" of perspective projection.
        // It is essential and will be used by the Rasterizer (Stage 3) for:
        // 1. Perspective Divide (ndc.x = clip.x / clip.w)
        // 2. Perspective-Correct Interpolation (using 1/w)
        //
        // A full implementation would be a 4x4 Projection Matrix multiplication:
        // Mat4x4 projMatrix = camera.GetProjectionMatrix(fov, aspect, near, far);
        // return projMatrix * Vec4(positionVS, 1.0f);

        float focal = 1.0f / std::tan(camera.fov / 2.0f);
        float oneOverNearMinusFar = 1.0f / (camera.nearPlane - camera.farPlane);

        float x = positionVS.x * focal / camera.aspectRatio;
        float y = positionVS.y * focal;
        float z = (positionVS.z * (camera.farPlane + camera.nearPlane) + 2 * camera.farPlane * camera.nearPlane) * oneOverNearMinusFar;
        float w = -positionVS.z;

        return Vec4(x, y, z, w);
    }

    inline float Smoothstep(float edge0, float edge1, float x)
    {
        x = std::max(0.0f, std::min(1.0f, (x - edge0) / (edge1 - edge0)));
        return x * x * (3 - 2 * x);
    }
}