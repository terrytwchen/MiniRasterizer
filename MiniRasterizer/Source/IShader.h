/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include <memory>
#include "PipelineData.h"
#include "Camera.h"
#include "Light.h"
#include "IShaderProperties.h"

class IShader
{
public:
    virtual ~IShader() = default;

    virtual VertexOutput RunVertexShader(
        const VertexInput& input,
        const Camera& camera,
        const Vec3& objectPosition
    ) const = 0;

    virtual Vec3 RunFragmentShader(
        const Fragment& fragment,
        const Camera& camera,
        const Light& light,
        const IShaderProperties& properties
    ) const = 0;

    virtual std::unique_ptr<IShaderProperties> CreateProperties() const = 0;
};