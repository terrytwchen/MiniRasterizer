#pragma once
#include "MeshData.h"
#include "Material.h"
#include "Vec3.h"
#include <memory>
#include <utility>
#include <stdexcept>

class RenderableObject
{
public:
    RenderableObject(std::shared_ptr<MeshData> mesh,
        std::shared_ptr<Material> material,
        const Vec3& position = Vec3(0, 0, 0))
        : _mesh(std::move(mesh)),
        _material(std::move(material)),
        _position(position)
    {
        if (!_mesh || !_material)
        {
            throw std::runtime_error("RenderableObject created with null mesh or material");
        }
    }

    ~RenderableObject() = default;

    void SetPosition(const Vec3& pos)
    {
        _position = pos;
    }

    void SetMesh(std::shared_ptr<MeshData> mesh)
    {
        if (!mesh) { throw std::runtime_error("Cannot set null mesh"); }
        _mesh = std::move(mesh);
    }

    void SetMaterial(std::shared_ptr<Material> material)
    {
        if (!material) { throw std::runtime_error("Cannot set null material"); }
        _material = std::move(material);
    }

    const Vec3& GetPosition() const
    {
        return _position;
    }

    std::shared_ptr<MeshData> GetMesh() const
    {
        return _mesh;
    }

    std::shared_ptr<Material> GetMaterial() const
    {
        return _material;
    }

private:
    Vec3 _position;
    std::shared_ptr<MeshData> _mesh;
    std::shared_ptr<Material> _material;
};