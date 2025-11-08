#pragma once
#include <memory>
#include <string>
#include <map>
#include <utility>

struct ShaderProperties
{
    virtual ~ShaderProperties() = default;

    // UI-Driving Methods
    virtual std::map<std::string, std::pair<float, float>> GetSliderProperties() const = 0;
    virtual std::string GetShaderName() const = 0;
};