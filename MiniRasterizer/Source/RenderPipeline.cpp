/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#include "RenderPipeline.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <limits>
#include <stdexcept>

RenderPipeline::RenderPipeline(int width, int height)
    : _width(width),
    _height(height)
{
    _InitializeDepthBuffer();
    _InitializeColorBuffer();
}

void RenderPipeline::SetCamera(const Camera& camera)
{
    _camera = camera;
}

void RenderPipeline::SetLight(const Light& light)
{
    _light = light;
}

void RenderPipeline::ClearBuffers()
{
    std::fill(_depthBuffer.begin(), _depthBuffer.end(), std::numeric_limits<float>::infinity());
    std::fill(_colorBuffer.begin(), _colorBuffer.end(), Vec3(0, 0, 0));
}

void RenderPipeline::Draw(const MeshData& mesh, const Vec3& objectPosition)
{
    if (!_boundShader || !_boundProperties)
    {
        throw std::runtime_error("Draw call failed: Shader or Properties not bound.");
    }

    _vertexOutputCache.clear();
    _RunVertexProcessing(
        mesh.GetPositions(),
        mesh.GetNormals(),
        objectPosition,
        _vertexOutputCache
    );

    _triangleCache.clear();
    _RunTriangleProcessing(
        _vertexOutputCache,
        mesh.GetIndices(),
        _triangleCache
    );

    _fragmentCache.clear();
    _RunRasterization(
        _triangleCache,
        _fragmentCache
    );

    _pixelCache.clear();
    _RunFragmentProcessing(
        _fragmentCache,
        _pixelCache
    );

    _RunFramebufferOperations(_pixelCache);
}

const std::vector<Vec3>& RenderPipeline::GetFinalColorBuffer() const
{
    return _colorBuffer;
}

void RenderPipeline::BindMaterial(Material* material)
{
    if (material)
    {
        _BindShader(material->GetShader());
        _BindProperties(material->GetProperties());
    }
    else
    {
        _BindShader(nullptr);
        _BindProperties(nullptr);
    }
}

void RenderPipeline::_BindShader(const IShader* shader)
{
    _boundShader = shader;
}

void RenderPipeline::_BindProperties(ShaderProperties* properties)
{
    _boundProperties = properties;
}

void RenderPipeline::_InitializeDepthBuffer()
{
    _depthBuffer.resize(_width * _height, std::numeric_limits<float>::infinity());
}

void RenderPipeline::_InitializeColorBuffer()
{
    _colorBuffer.resize(_width * _height, Vec3(0, 0, 0));
}

// Pipeline Stages
void RenderPipeline::_RunVertexProcessing(
    const std::vector<Vec3>& positions,
    const std::vector<Vec3>& normals,
    const Vec3& objectPosition,
    std::vector<VertexOutput>& outVertexOutputs
) const
{
    outVertexOutputs.reserve(positions.size());

    for (size_t i = 0; i < positions.size(); ++i)
    {
        VertexInput vertexInput;
        vertexInput.positionMS = positions[i];
        vertexInput.normalMS = normals[i];

        VertexOutput vertexOutput = _boundShader->RunVertexShader(
            vertexInput,
            _camera,
            objectPosition
        );

        outVertexOutputs.push_back(vertexOutput);
    }
}

void RenderPipeline::_RunTriangleProcessing(
    const std::vector<VertexOutput>& vertexOutputs,
    const std::vector<unsigned int>& indices,
    std::vector<TrianglePrimitive>& outTriangles
) const
{
    outTriangles.reserve(indices.size() / 3);

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        if (i0 >= vertexOutputs.size() ||
            i1 >= vertexOutputs.size() ||
            i2 >= vertexOutputs.size())
        {
            continue;
        }

        const VertexOutput& v0 = vertexOutputs[i0];
        const VertexOutput& v1 = vertexOutputs[i1];
        const VertexOutput& v2 = vertexOutputs[i2];

        outTriangles.push_back(TrianglePrimitive{ v0, v1, v2 });
    }
}

void RenderPipeline::_RunRasterization(
    const std::vector<TrianglePrimitive>& trianglePrimitives,
    std::vector<Fragment>& outFragments
) const
{
    outFragments.reserve(trianglePrimitives.size() * 100);
    for (const TrianglePrimitive& triangle : trianglePrimitives)
    {
        if (_IsFrustumCulled(triangle))
        {
            continue;
        }
        _RasterizeSingleTriangle(triangle, outFragments);
    }
}

bool RenderPipeline::_IsFrustumCulled(
    const TrianglePrimitive& triangle
) const
{
    if (triangle.v0.positionCS.w <= 0.001f ||
        triangle.v1.positionCS.w <= 0.001f ||
        triangle.v2.positionCS.w <= 0.001f)
    {
        return true;
    }
    return false;
}

void RenderPipeline::_RasterizeSingleTriangle(
    const TrianglePrimitive& tri,
    std::vector<Fragment>& outFragments
) const
{
    Vec3 ndc0 = Vec3(tri.v0.positionCS.x / tri.v0.positionCS.w,
        tri.v0.positionCS.y / tri.v0.positionCS.w,
        tri.v0.positionCS.z / tri.v0.positionCS.w);
    Vec3 ndc1 = Vec3(tri.v1.positionCS.x / tri.v1.positionCS.w,
        tri.v1.positionCS.y / tri.v1.positionCS.w,
        tri.v1.positionCS.z / tri.v1.positionCS.w);
    Vec3 ndc2 = Vec3(tri.v2.positionCS.x / tri.v2.positionCS.w,
        tri.v2.positionCS.y / tri.v2.positionCS.w,
        tri.v2.positionCS.z / tri.v2.positionCS.w);

    Vec3 p0_ss = Vec3(
        (ndc0.x + 1.0f) * 0.5f * _width,
        (1.0f - ndc0.y) * 0.5f * _height,
        ndc0.z
    );
    Vec3 p1_ss = Vec3(
        (ndc1.x + 1.0f) * 0.5f * _width,
        (1.0f - ndc1.y) * 0.5f * _height,
        ndc1.z
    );
    Vec3 p2_ss = Vec3(
        (ndc2.x + 1.0f) * 0.5f * _width,
        (1.0f - ndc2.y) * 0.5f * _height,
        ndc2.z
    );

    int minX = std::max(0, static_cast<int>(std::floor(std::min({ p0_ss.x, p1_ss.x, p2_ss.x }))));
    int maxX = std::min(_width - 1, static_cast<int>(std::ceil(std::max({ p0_ss.x, p1_ss.x, p2_ss.x }))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({ p0_ss.y, p1_ss.y, p2_ss.y }))));
    int maxY = std::min(_height - 1, static_cast<int>(std::ceil(std::max({ p0_ss.y, p1_ss.y, p2_ss.y }))));

    float inv_w0 = 1.0f / tri.v0.positionCS.w;
    float inv_w1 = 1.0f / tri.v1.positionCS.w;
    float inv_w2 = 1.0f / tri.v2.positionCS.w;

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            Vec3 p_pixel(x + 0.5f, y + 0.5f, 0);
            Vec3 bary = _ComputeBarycentricCoords(p_pixel, p0_ss, p1_ss, p2_ss);

            if (bary.x < -0.001f || bary.y < -0.001f || bary.z < -0.001f)
            {
                continue;
            }

            float z_depth = bary.x * p0_ss.z + bary.y * p1_ss.z + bary.z * p2_ss.z;

            Varyings interpolatedVaryings = _InterpolateVaryings(
                tri.v0.varyings, tri.v1.varyings, tri.v2.varyings,
                inv_w0, inv_w1, inv_w2,
                bary
            );

            Fragment frag;
            frag.x = x;
            frag.y = y;
            frag.z_depth = z_depth;
            frag.interpolatedVaryings = interpolatedVaryings;

            outFragments.push_back(frag);
        }
    }
}

Vec3 RenderPipeline::_ComputeBarycentricCoords(
    const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c
) const
{
    Vec3 v0 = b - a; v0.z = 0;
    Vec3 v1 = c - a; v1.z = 0;
    Vec3 v2 = p - a; v2.z = 0;

    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);

    float denom = d00 * d11 - d01 * d01;

    if (std::abs(denom) < 1e-6f)
    {
        return Vec3(-1, -1, -1);
    }

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return Vec3(u, v, w);
}

Varyings RenderPipeline::_InterpolateVaryings(
    const Varyings& v0, const Varyings& v1, const Varyings& v2,
    float inv_w0, float inv_w1, float inv_w2,
    const Vec3& bary
) const
{
    float z_inv_pixel = bary.x * inv_w0 + bary.y * inv_w1 + bary.z * inv_w2;
    float z_pixel_correction = 1.0f / z_inv_pixel;

    Vec3 posVS_interp =
        (v0.positionVS * inv_w0 * bary.x) +
        (v1.positionVS * inv_w1 * bary.y) +
        (v2.positionVS * inv_w2 * bary.z);

    Vec3 normVS_interp =
        (v0.normalVS * inv_w0 * bary.x) +
        (v1.normalVS * inv_w1 * bary.y) +
        (v2.normalVS * inv_w2 * bary.z);

    Varyings finalVaryings;
    finalVaryings.positionVS = posVS_interp * z_pixel_correction;
    finalVaryings.normalVS = normVS_interp * z_pixel_correction;

    return finalVaryings;
}

void RenderPipeline::_RunFragmentProcessing(
    const std::vector<Fragment>& fragments,
    std::vector<PixelData>& outPixelDatas
) const
{
    outPixelDatas.reserve(fragments.size());

    for (const Fragment& frag : fragments)
    {
        Vec3 shadedColor = _boundShader->RunFragmentShader(
            frag,
            _camera,
            _light,
            *_boundProperties
        );

        PixelData pixelData;
        pixelData.x = frag.x;
        pixelData.y = frag.y;
        pixelData.z_depth = frag.z_depth;
        pixelData.color = shadedColor;

        outPixelDatas.push_back(pixelData);
    }
}

void RenderPipeline::_RunFramebufferOperations(
    const std::vector<PixelData>& shadedPixels
)
{
    for (const auto& pixel : shadedPixels)
    {
        int index = pixel.y * _width + pixel.x;

        if (pixel.x < 0 || pixel.x >= _width || pixel.y < 0 || pixel.y >= _height)
        {
            continue;
        }

        if (pixel.z_depth < _depthBuffer[index])
        {
            _depthBuffer[index] = pixel.z_depth;
            _colorBuffer[index] = pixel.color;
        }
    }
}