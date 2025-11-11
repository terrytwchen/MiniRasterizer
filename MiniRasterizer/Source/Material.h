/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include "IShader.h"
#include "ShaderProperties.h"
#include <memory>
#include <utility>
#include <stdexcept>

class Material
{
public:
    Material(std::shared_ptr<IShader> shader)
        : _shader(std::move(shader))
    {
        if (!_shader)
        {
            throw std::runtime_error("Material constructed with null shader.");
        }
        _properties = _shader->CreateProperties();
    }

    void SetShader(std::shared_ptr<IShader> shader)
    {
        if (!shader)
        {
            throw std::runtime_error("Cannot set null shader to Material.");
        }
        _shader = std::move(shader);
        _properties = _shader->CreateProperties();
    }

    const IShader* GetShader() const
    {
        return _shader.get();
    }

    ShaderProperties* GetProperties() const
    {
        return _properties.get();
    }

private:
    std::shared_ptr<IShader> _shader;
    std::unique_ptr<ShaderProperties> _properties;
};