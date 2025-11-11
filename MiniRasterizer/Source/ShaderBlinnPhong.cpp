/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#include "ShaderBlinnPhong.h"
#include "BlinnPhongProperties.h"
#include "ShaderUtils.h"
#include <algorithm>
#include <cmath>

VertexOutput ShaderBlinnPhong::RunVertexShader(
    const VertexInput& input,
    const Camera& camera,
    const Vec3& objectPosition
) const
{
    // 1. Model-to-World Transform
    // We are calling our simplified Vec3-based function.
    Vec3 worldPos = ShaderUtils::TransformModelToWorld(input.positionMS, objectPosition);

    // [SIMPLIFICATION]
    // A normal vector (a direction) should be transformed differently than a position. It should use the inverse-transpose of the model matrix and 'w' component should be 0.
    // For now, we assume no rotation/scale, so we just pass it along.
    Vec3 normalWS = input.normalMS;

    // 2. World-to-View Transform
    // Calling our simplified Vec3-based function.
    Vec3 viewPos = ShaderUtils::TransformWorldToView(worldPos, camera);

    // [SIMPLIFICATION]
    // Transforming the normal to View Space. This is also simplified.
    Vec3 normalVS = ShaderUtils::TransformWorldToView(normalWS + worldPos, camera) - viewPos;
    normalVS = normalVS.normalize();

    // 3. View-to-Clip Transform
    // This is the CRITICAL step where our Vec3 'viewPos' is converted into the essential Vec4 'clipPos' (with 'w').
    Vec4 clipPos = ShaderUtils::TransformViewToClip(viewPos, camera);

    // 4. Pack Varyings
    Varyings varyings;
    varyings.positionVS = viewPos;
    varyings.normalVS = normalVS;

    // The final output MUST be a VertexOutput containing the Vec4 clipPos
    return VertexOutput{ clipPos, varyings };
}

Vec3 ShaderBlinnPhong::RunFragmentShader(
    const Fragment& fragment,
    const Camera& camera,
    const Light& light,
    const ShaderProperties& properties
) const
{
    // 1. Downcast to get shader properties
    const auto& props = static_cast<const BlinnPhongProperties&>(properties);

    // 2. Get interpolated data (varyings)
    const Varyings& varyings = fragment.interpolatedVaryings;

    // 3. Prepare lighting vectors (all in View Space)
    Vec3 N = varyings.normalVS.normalize();
    Vec3 V = (Vec3(0, 0, 0) - varyings.positionVS).normalize(); // Camera position in View Space is always (0,0,0)
    Vec3 L_vs = ShaderUtils::TransformWorldToView(light.position, camera); // Transform Light position to View Space
    L_vs = (L_vs - varyings.positionVS).normalize();

    Vec3 H = (L_vs + V).normalize();

    // 4. Compute lighting (Blinn-Phong)
    float NdotL = std::max(0.0f, N.dot(L_vs));
    float NdotH = std::max(0.0f, N.dot(H));

    Vec3 ambient = props.ambient;
    Vec3 diffuse = props.diffuse * NdotL;
    Vec3 specular = props.specular * std::pow(NdotH, props.smoothness);

    Vec3 finalColor = (ambient + diffuse + specular) * light.color;

    return finalColor;
}

std::unique_ptr<ShaderProperties> ShaderBlinnPhong::CreateProperties() const
{
    return std::make_unique<BlinnPhongProperties>();
}