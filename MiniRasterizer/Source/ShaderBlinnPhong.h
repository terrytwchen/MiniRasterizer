/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include "IShader.h"

class ShaderBlinnPhong : public IShader
{
public:
    virtual VertexOutput RunVertexShader(
        const VertexInput& input,
        const Camera& camera,
        const Vec3& objectPosition
    ) const override;

    virtual Vec3 RunFragmentShader(
        const Fragment& fragment,
        const Camera& camera,
        const Light& light,
        const ShaderProperties& properties
    ) const override;

    virtual std::unique_ptr<ShaderProperties> CreateProperties() const override;
};