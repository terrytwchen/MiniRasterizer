#pragma once
#include <memory>
#include "PipelineData.h"
#include "Camera.h"
#include "Light.h"
#include "ShaderProperties.h"

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
        const ShaderProperties& properties
    ) const = 0;

    virtual std::unique_ptr<ShaderProperties> CreateProperties() const = 0;
};