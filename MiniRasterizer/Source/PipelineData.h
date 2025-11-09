#pragma once
#include "Vec3.h"
#include "Vec4.h"

// Read from MeshData, will be sent in to Vertex Shader
struct VertexInput
{
    Vec3 positionMS;
    Vec3 normalMS;
    // TODO: Vec2 uv;
};

// Data need to do interpolation in Rasterization process
struct Varyings
{
    Vec3 positionVS;
    Vec3 normalVS;
    // TODO: Vec2 uv;
};

// The actual output for Vertex Shader.
struct VertexOutput
{
    Vec4 positionCS;
    Varyings varyings;
};

// The output from Rasterization process, will be input for Fragment Shader
struct Fragment
{
    int x = 0;
    int y = 0;
    float z_depth = 0.0f;
    Varyings interpolatedVaryings;
};

// A Triangle composite of 3 VertexOutput
struct TrianglePrimitive
{
    VertexOutput v0;
    VertexOutput v1;
    VertexOutput v2;
};

// Final data written to color buffer
struct PixelData
{
    int x = 0;
    int y = 0;
    float z_depth = 0.0f;
    Vec3 color;
};