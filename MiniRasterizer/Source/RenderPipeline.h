/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include <vector>
#include <memory>
#include <string>

#include "Vec3.h"
#include "Camera.h"
#include "Light.h"
#include "IShader.h"
#include "Material.h"
#include "ShaderProperties.h"
#include "MeshData.h"
#include "PipelineData.h"

class RenderPipeline
{
public:
    RenderPipeline(int width, int height);
    ~RenderPipeline() = default;

    void SetCamera(const Camera& camera);
    void SetLight(const Light& light);
    void ClearBuffers();
    void Draw(const MeshData& mesh, const Vec3& objectPosition);
    const std::vector<Vec3>& GetFinalColorBuffer() const;
    void BindMaterial(Material* material);

    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }

private:
    void _BindShader(const IShader* shader);
    void _BindProperties(ShaderProperties* properties);
    void _InitializeDepthBuffer();
    void _InitializeColorBuffer();

    // Pipeline Stages
    void _RunVertexProcessing(
        const std::vector<Vec3>& positions,
        const std::vector<Vec3>& normals,
        const Vec3& objectPosition,
        std::vector<VertexOutput>& outVertexOutputs
    ) const;

    void _RunTriangleProcessing(
        const std::vector<VertexOutput>& vertexOutputs,
        const std::vector<unsigned int>& indices,
        std::vector<TrianglePrimitive>& outTriangles
    ) const;

    void _RunRasterization(
        const std::vector<TrianglePrimitive>& trianglePrimitives,
        std::vector<Fragment>& outFragments
    ) const;

    bool _IsFrustumCulled(
        const TrianglePrimitive& triangle
    ) const;

    void _RasterizeSingleTriangle(
        const TrianglePrimitive& tri,
        std::vector<Fragment>& outFragments
    ) const;

    Vec3 _ComputeBarycentricCoords(
        const Vec3& p,
        const Vec3& a,
        const Vec3& b,
        const Vec3& c
    ) const;

    Varyings _InterpolateVaryings(
        const Varyings& v0, const Varyings& v1, const Varyings& v2,
        float inv_w0, float inv_w1, float inv_w2,
        const Vec3& barycentricCoords
    ) const;

    void _RunFragmentProcessing(
        const std::vector<Fragment>& fragments,
        std::vector<PixelData>& outPixelDatas
    ) const;

    void _RunFramebufferOperations(
        const std::vector<PixelData>& shadedPixels
    );

    // Member Data
    int _width;
    int _height;

    std::vector<float> _depthBuffer;
    std::vector<Vec3> _colorBuffer;

    Camera _camera;
    Light _light;

    const IShader* _boundShader = nullptr;
    ShaderProperties* _boundProperties = nullptr;

    // Caches
    std::vector<VertexOutput> _vertexOutputCache;
    std::vector<TrianglePrimitive> _triangleCache;
    std::vector<Fragment> _fragmentCache;
    std::vector<PixelData> _pixelCache;
};