#pragma once
#include <cmath>
#include "MeshData.h"
#include <memory>
#include <vector>

namespace MeshGenerator
{
    static constexpr float PI = 3.14159265358979323846f;

    inline std::shared_ptr<MeshData> CreateSphere(float radius, unsigned int segments, const Vec3& posOffset)
    {
        std::vector<Vec3> positions;
        std::vector<Vec3> normals;
        std::vector<unsigned int> indices;

        // === 1. Generate Vertices and Normals ===
        // Iterate over vertical slices (latitude)
        for (unsigned int y = 0; y <= segments; ++y)
        {
            // Iterate over horizontal slices (longitude)
            for (unsigned int x = 0; x <= segments; ++x)
            {
                float phi = static_cast<float>(y) * PI / segments;
                float theta = static_cast<float>(x) * 2.0f * PI / segments;

                // Convert spherical (radius, phi, theta) to Cartesian (x, y, z)
                float xPos = radius * std::sin(phi) * std::cos(theta);
                float yPos = radius * std::cos(phi);
                float zPos = radius * std::sin(phi) * std::sin(theta);

                positions.emplace_back(xPos + posOffset.x, yPos + posOffset.y, zPos + posOffset.z);
                normals.emplace_back(Vec3(xPos, yPos, zPos).normalize());
            }
        }

        // === 2. Generate Indices (Triangles) ===
        // Iterate over the quads formed by the grid (excluding the last row/column)
        for (unsigned int y = 0; y < segments; ++y)
        {
            for (unsigned int x = 0; x < segments; ++x)
            {
                // Get the indices of the four corners of the current quad
                // (segments + 1) is the width of the grid (number of vertices per row)
                unsigned int i0 = y * (segments + 1) + x;
                unsigned int i1 = i0 + 1;
                unsigned int i2 = (y + 1) * (segments + 1) + x;
                unsigned int i3 = i2 + 1;

                // Create the first triangle (i0, i2, i1) - Clockwise
                // (This is the "upper-left" half of the quad)
                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                // Create the second triangle (i1, i2, i3) - Clockwise
                // (This is the "bottom-right" half of the quad)
                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        return std::make_shared<MeshData>(std::move(positions), std::move(normals), std::move(indices));
    }
}