/* MiniRasterizer
 * Copyright (c) 2025 terrytw. Licensed under the MIT License.
 * See LICENSE file for details.
 */

#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "Vec3.h"

class MeshData
{
private:
    std::vector<Vec3> _positions;
    std::vector<Vec3> _normals;
    std::vector<unsigned int> _indices;

public:
    MeshData(std::vector<Vec3> positions,
        std::vector<Vec3> normals,
        std::vector<unsigned int> indices)
        : _positions(std::move(positions)),
        _normals(std::move(normals)),
        _indices(std::move(indices))
    {
        if (_positions.size() != _normals.size())
        {
            throw std::runtime_error("MeshData: Positions and normals count mismatch.");
        }
    }

    ~MeshData() = default;
    MeshData(const MeshData&) = default;
    MeshData(MeshData&&) = default;
    MeshData& operator=(const MeshData&) = default;
    MeshData& operator=(MeshData&&) = default;

    const std::vector<Vec3>& GetPositions() const
    {
        return _positions;
    }

    const std::vector<Vec3>& GetNormals() const
    {
        return _normals;
    }

    const std::vector<unsigned int>& GetIndices() const
    {
        return _indices;
    }
};