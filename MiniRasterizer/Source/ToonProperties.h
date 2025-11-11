/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include "ShaderProperties.h"
#include "Vec3.h"
#include <string>
#include <map>
#include <utility>

struct ToonProperties : public ShaderProperties
{
    Vec3 baseColor{ 0.7f, 0.2f, 0.2f };
    Vec3 ambient{ 0.1f, 0.1f, 0.1f };
    Vec3 rimColor{ 1.0f, 1.0f, 1.0f };
    float softness{ 0.2f };
    float rimWidth{ 0.5f };
    float rimSoftness{ 0.1f };
    float rimDirection{ 1.0f };

    std::map<std::string, std::pair<float, float>> GetSliderProperties() const override
    {
        return
        {
            {"AmbientX_Red", {0.0f, 1.0f}},
            {"AmbientY_Green", {0.0f, 1.0f}},
            {"AmbientZ_Blue", {0.0f, 1.0f}},
            {"BaseColorX_Red", {0.0f, 1.0f}},
            {"BaseColorY_Green", {0.0f, 1.0f}},
            {"BaseColorZ_Blue", {0.0f, 1.0f}},
            {"DiffuseSoftness", {0.01f, 0.5f}},
            {"RimColorX_Red", {0.0f, 1.0f}},
            {"RimColorY_Green", {0.0f, 1.0f}},
            {"RimColorZ_Blue", {0.0f, 1.0f}},
            {"RimWidth", {0.0f, 1.0f}},
            {"RimSoftness", {0.01f, 0.5f}},
            {"RimDirection", {-1.0f, 1.0f}}
        };
    }

    std::string GetShaderName() const override
    {
        return "Simple Toon Shader";
    }
};