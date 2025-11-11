/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#include "ShaderToon.h"
#include "ToonProperties.h"
#include "ShaderUtils.h"
#include <algorithm>
#include <cmath>

VertexOutput ShaderToon::RunVertexShader(
    const VertexInput& input,
    const Camera& camera,
    const Vec3& objectPosition
) const
{
    Vec3 worldPos = ShaderUtils::TransformModelToWorld(input.positionMS, objectPosition);
    Vec3 normalWS = input.normalMS;
    Vec3 viewPos = ShaderUtils::TransformWorldToView(worldPos, camera);
    Vec3 normalVS = ShaderUtils::TransformWorldToView(normalWS + worldPos, camera) - viewPos;
    normalVS = normalVS.normalize();
    Vec4 clipPos = ShaderUtils::TransformViewToClip(viewPos, camera);

    Varyings varyings;
    varyings.positionVS = viewPos;
    varyings.normalVS = normalVS;

    return VertexOutput{ clipPos, varyings };
}

Vec3 ShaderToon::RunFragmentShader(
    const Fragment& fragment,
    const Camera& camera,
    const Light& light,
    const ShaderProperties& properties
) const
{
    const auto& props = static_cast<const ToonProperties&>(properties);
    const Varyings& varyings = fragment.interpolatedVaryings;

    Vec3 N = varyings.normalVS.normalize();
    Vec3 V = (Vec3(0, 0, 0) - varyings.positionVS).normalize();
    Vec3 L_vs = ShaderUtils::TransformWorldToView(light.position, camera);
    L_vs = (L_vs - varyings.positionVS).normalize();

    // Toon shading calculation
    float NdotL = std::max(0.0f, N.dot(L_vs));
    float halfSoftness = props.softness * 0.5f;
    float toonDiffuse = ShaderUtils::Smoothstep(0.5f - halfSoftness, 0.5f + halfSoftness, NdotL);

    // Rim light
    float NdotV = std::max(0.0f, N.dot(V));
    float rimFactor = 1.0f - NdotV;

    float directionMask;
    if (props.rimDirection > 0)
    {
        directionMask = NdotL; // Light-side
    }
    else if (props.rimDirection < 0)
    {
        directionMask = 1.0f - NdotL; // Dark-side
    }
    else
    {
        directionMask = 1.0f; // No preference
    }

    float directionStrength = std::abs(props.rimDirection);
    float finalRimFactor = rimFactor * (directionMask * directionStrength + (1.0f - directionStrength));

    float rimThreshold = 1.0f - props.rimWidth;
    float rimAmount = ShaderUtils::Smoothstep(rimThreshold - props.rimSoftness,
        rimThreshold + props.rimSoftness,
        finalRimFactor);

    Vec3 base = props.ambient + props.baseColor * toonDiffuse;
    Vec3 fragColor = base * (1.0f - rimAmount) + props.rimColor * rimAmount;
    fragColor = fragColor * light.color;

    return fragColor;
}

std::unique_ptr<ShaderProperties> ShaderToon::CreateProperties() const
{
    return std::make_unique<ToonProperties>();
}