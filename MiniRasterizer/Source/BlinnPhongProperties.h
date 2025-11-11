/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include "IShaderProperties.h"
#include "Vec3.h"
#include <string>
#include <map>
#include <utility>

struct BlinnPhongProperties : public IShaderProperties
{
    Vec3 ambient{ 0.1f, 0.1f, 0.1f };
    Vec3 diffuse{ 0.7f, 0.2f, 0.2f };
    Vec3 specular{ 1.0f, 1.0f, 1.0f };
    float smoothness{ 32.0f };

    // UI-Driving Methods
    std::map<std::string, std::pair<float, float>> GetSliderProperties() const override
    {
        return
        {
            {"AmbientX_Red", {0.0f, 1.0f}},
            {"AmbientY_Green", {0.0f, 1.0f}},
            {"AmbientZ_Blue", {0.0f, 1.0f}},
            {"DiffuseX_Red", {0.0f, 1.0f}},
            {"DiffuseY_Green", {0.0f, 1.0f}},
            {"DiffuseZ_Blue", {0.0f, 1.0f}},
            {"SpecularX_Red", {0.0f, 1.0f}},
            {"SpecularY_Green", {0.0f, 1.0f}},
            {"SpecularZ_Blue", {0.0f, 1.0f}},
            {"Smoothness", {1.0f, 64.0f}}
        };
    }

    std::string GetShaderName() const override
    {
        return "Blinn-Phong Shader";
    }
};