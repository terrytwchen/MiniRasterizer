#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

//>>>>>>>>>>>>>>>>>>>>>>>>>Global Variable, functions, and structs>>>>>>>>>>>>>>>>>>>>>>>>>//
#define M_PI 3.14159265358979323846

float smoothstep(float edge0, float edge1, float x)
{
    x = std::max(0.0f, std::min(1.0f, (x - edge0) / (edge1 - edge0)));
    return x * x * (3 - 2 * x);
}

struct Vec3
{
public:
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vec3 normalize() const
    {
        float len = std::sqrt(x * x + y * y + z * z);
        return Vec3(x / len, y / len, z / len);
    }

    float dot(const Vec3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 operator*(const float& other) const
    {
        return Vec3(x * other, y * other, z * other);
    }

    Vec3 operator*(const Vec3& other) const
    {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    Vec3 operator+(const Vec3& other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>Property Enums>>>>>>>>>>>>>>>>>>>>>>>>>//

enum class BlinnPhongProperty
{
    AmbientX_Red, AmbientY_Green, AmbientZ_Blue,
    DiffuseX_Red, DiffuseY_Green, DiffuseZ_Blue,
    SpecularX_Red, SpecularY_Green, SpecularZ_Blue,
    Smoothness
};

enum class ToonProperty
{
    AmbientX_Red, AmbientY_Green, AmbientZ_Blue,
    BaseColorX_Red, BaseColorY_Green, BaseColorZ_Blue,
    RimColorX_Red, RimColorY_Green, RimColorZ_Blue,
    DiffuseSoftness,
    RimWidth,
    RimSoftness,
    RimDirection
};

BlinnPhongProperty GetPropertyEnum(const std::string& prop)
{
    static const std::map<std::string, BlinnPhongProperty> propertyMap = {
        {"AmbientX_Red", BlinnPhongProperty::AmbientX_Red},
        {"AmbientY_Green", BlinnPhongProperty::AmbientY_Green},
        {"AmbientZ_Blue", BlinnPhongProperty::AmbientZ_Blue},
        {"DiffuseX_Red", BlinnPhongProperty::DiffuseX_Red},
        {"DiffuseY_Green", BlinnPhongProperty::DiffuseY_Green},
        {"DiffuseZ_Blue", BlinnPhongProperty::DiffuseZ_Blue},
        {"SpecularX_Red", BlinnPhongProperty::SpecularX_Red},
        {"SpecularY_Green", BlinnPhongProperty::SpecularY_Green},
        {"SpecularZ_Blue", BlinnPhongProperty::SpecularZ_Blue},
        {"Smoothness", BlinnPhongProperty::Smoothness}
    };
    return propertyMap.at(prop);
}

ToonProperty GetToonPropertyEnum(const std::string& prop)
{
    static const std::map<std::string, ToonProperty> propertyMap = {
        {"AmbientX_Red", ToonProperty::AmbientX_Red},
        {"AmbientY_Green", ToonProperty::AmbientY_Green},
        {"AmbientZ_Blue", ToonProperty::AmbientZ_Blue},
        {"BaseColorX_Red", ToonProperty::BaseColorX_Red},
        {"BaseColorY_Green", ToonProperty::BaseColorY_Green},
        {"BaseColorZ_Blue", ToonProperty::BaseColorZ_Blue},
        {"DiffuseSoftness", ToonProperty::DiffuseSoftness},
        {"RimColorX_Red", ToonProperty::RimColorX_Red},
        {"RimColorY_Green", ToonProperty::RimColorY_Green},
        {"RimColorZ_Blue", ToonProperty::RimColorZ_Blue},
        {"RimWidth", ToonProperty::RimWidth},
        {"RimSoftness", ToonProperty::RimSoftness},
        {"RimDirection", ToonProperty::RimDirection }
    };
    return propertyMap.at(prop);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>Material Properties>>>>>>>>>>>>>>>>>>>>>>>>>//

struct Camera {
    Vec3 position;
    Vec3 direction;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera(Vec3 pos = Vec3(0.0f, 0.0f, 10.0f),
        Vec3 dir = Vec3(0.0f, 0.0f, -1.0f),
        float fov = 60.0f,
        float aspect = 960.0f / 540.0f,
        float near = 0.1f,
        float far = 100.0f)
        : position(pos), direction(dir.normalize()),
        fov(fov* M_PI / 180.0f), aspectRatio(aspect),
        nearPlane(near), farPlane(far)
    {}
};

struct Light
{
    Vec3 position;
    Vec3 color;
};

struct ShaderProperties
{
    virtual ~ShaderProperties() = default;
    virtual std::map<std::string, std::pair<float, float>> GetSliderProperties() const = 0;
    virtual std::string GetShaderName() const = 0;
};

struct BlinnPhongProperties : public ShaderProperties
{
    Vec3 ambient{ 0.1f, 0.1f, 0.1f };
    Vec3 diffuse{ 0.7f, 0.2f, 0.2f };
    Vec3 specular{ 1.0f, 1.0f, 1.0f };
    float smoothness{ 32.0f };

    std::map<std::string, std::pair<float, float>> GetSliderProperties() const override
    {
        return
        {
            {"AmbientX_Red", {0.0f, 1.0f}},
            {"AmbientY_Green", {0.0f, 1.0f}},
            {"AmbientZ_Blue", {0.0f, 1.0f}},
            {"DiffuseX_Red", {0.0f, 1.0f}},
            {"DiffuseY_Green", {0.0f, 1.0f}},
            {"DiffuseZ_Blue", {0.0f, 1.0f}},
            {"SpecularX_Red", {0.0f, 1.0f}},
            {"SpecularY_Green", {0.0f, 1.0f}},
            {"SpecularZ_Blue", {0.0f, 1.0f}},
            {"Smoothness", {1.0f, 64.0f}}
        };
    }

    std::string GetShaderName() const override
    {
        return "Blinn-Phong Shader";
    }
};

struct ToonProperties : public ShaderProperties
{
    Vec3 baseColor{ 0.7f, 0.2f, 0.2f };
    Vec3 ambient{ 0.1f, 0.1f, 0.1f };    // Ambient color
    Vec3 rimColor{ 1.0f, 1.0f, 1.0f };   // Rim light color
    float softness{ 0.2f };              // Controls the width of the transition between light and shadow
    float rimWidth{ 0.5f };              // Controls how far the rim light extends from the edge
    float rimSoftness{ 0.1f };           // Controls the softness of the rim light edge
    float rimDirection{ 1.0f };          // Controls the rimlight direction

    std::map<std::string, std::pair<float, float>> GetSliderProperties() const override
    {
        return
        {
            {"AmbientX_Red", {0.0f, 1.0f}},
            {"AmbientY_Green", {0.0f, 1.0f}},
            {"AmbientZ_Blue", {0.0f, 1.0f}},
            {"BaseColorX_Red", {0.0f, 1.0f}},
            {"BaseColorY_Green", {0.0f, 1.0f}},
            {"BaseColorZ_Blue", {0.0f, 1.0f}},
            {"DiffuseSoftness", {0.01f, 0.5f}},
            {"RimColorX_Red", {0.0f, 1.0f}},
            {"RimColorY_Green", {0.0f, 1.0f}},
            {"RimColorZ_Blue", {0.0f, 1.0f}},
            {"RimWidth", {0.0f, 1.0f}},
            {"RimSoftness", {0.01f, 0.5f}},
            {"RimDirection", {-1.0f, 1.0f}}
        };
    }

    std::string GetShaderName() const override
    {
        return "Simple Toon Shader";
    }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>Shaders>>>>>>>>>>>>>>>>>>>>>>>>>//

class Shader
{
protected:
    Vec3 TransformPositionToClipSpace(const Vec3& pos, const Camera& camera)
    {
        // Calculate perspective projection matrix components
        float f = 1.0f / std::tan(camera.fov / 2.0f);
        float nf = 1.0f / (camera.nearPlane - camera.farPlane);

        // Move vertex relative to camera position (0,0,10)
        // Effectively moves camera to origin and adjusts scene accordingly
        //TODO: sync with the camera initialized in main program
        Vec3 viewPos = pos - camera.position;

        // Perspective projection transformation
        // Project x and y based on FOV and aspect ratio
        float x = viewPos.x * f / camera.aspectRatio;
        float y = viewPos.y * f;

        // Calculate z-depth for perspective projection
        // Maps z to the range suitable for depth testing
        float z = (((camera.farPlane + camera.nearPlane) * viewPos.z + 2 * camera.farPlane * camera.nearPlane) * nf);

        // Store negative z for perspective divide
        // This creates the perspective effect where distant objects appear smaller
        float w = -viewPos.z;

        // Perspective division and screen space adjustment
        // Only divide if w is not too close to zero to avoid division by zero
        if (std::abs(w) > 0.0001f)
        {
            x /= w;     // Divide x by w for perspective
            y /= -w;    // Flip y-coordinate for SFML
            z /= w;     // Divide z by w for correct depth
        }

        // Return final screen space coordinates
        // x: [-1,1] maps to [0,width]
        // y: [-1,1] maps to [height,0] (SFML's top-left origin)
        // z: Preserved for depth testing
        return Vec3(x, y, z);
    }

    Vec3 TransformPositionToViewSpace(const Vec3& pos, const Camera& camera)
    {
        // Simple view transform (camera at 0,0,10)
        //TODO: sync with the camera initialized in main program
        return pos - camera.position;
    }

    Vec3 TransformNormalToViewSpace(const Vec3& normal, const Vec3& position, const Camera& camera)
    {
        // For a sphere, instead of using the vertex normal directly,
        // we should calculate it from the vertex position relative to sphere center
        // Use view space position and view space coordinates to calculate the normal in view space.
        
        Vec3 viewSpacePosition = TransformPositionToViewSpace(position, camera);
        Vec3 viewSpaceSphereCenter = TransformPositionToViewSpace(Vec3(0, 0, 0), camera);

        // Normal in view space should point from center to surface
        return (viewSpacePosition - viewSpaceSphereCenter).normalize();
    }

public:
    struct VertexOutput
    {
        Vec3 positionCS;     // Projected position
        Vec3 positionVS;     // View space position for lighting
        Vec3 normalVS;       // View space normal for lighting
    };
    virtual ShaderProperties* GetProperties() = 0;
    virtual VertexOutput VertexShader(const Vec3& position, const Vec3& normal, const Camera& camera) = 0;
    virtual sf::Color FragmentShader(const VertexOutput& vertexData, const Light& light, const Camera& camera) = 0;
};


class ShaderBlinnPhong : public Shader
{
private:
    BlinnPhongProperties properties;
public:
    ShaderProperties* GetProperties() { return &properties; }

    VertexOutput VertexShader(const Vec3& position, const Vec3& normal, const Camera& camera) override
    {
        VertexOutput output;

        // Transform vertex to view space
        Vec3 viewPos = TransformPositionToViewSpace(position, camera);

        // For a sphere, recalculate normal in view space based on position
        Vec3 viewNormal = TransformNormalToViewSpace(normal, position, camera);

        output.positionVS = viewPos;     // Store view space position
        output.normalVS = viewNormal;    // Store view space normal
        output.positionCS = TransformPositionToClipSpace(position, camera);

        return output;
    }

    sf::Color FragmentShader(const VertexOutput& vertexData,
        const Light& light,
        const Camera& camera) override
    {
        // Transform light to view space
        Vec3 lightPosVS = TransformPositionToViewSpace(light.position, camera);

        // Get normalized vectors
        Vec3 normal = vertexData.normalVS.normalize();
        Vec3 lightDirVS = (lightPosVS - vertexData.positionVS).normalize();
        Vec3 viewDirVS = (camera.position - vertexData.positionVS).normalize();

        // Calculate half vector for Blinn-Phong
        Vec3 halfVec = (lightDirVS + viewDirVS).normalize();

        // Calculate dot products with clamping to avoid numerical issues
        float NdotL = std::max(normal.dot(lightDirVS), 0.0f);
        float NdotH = std::max(normal.dot(halfVec), 0.0f);

        // Calculate lighting components
        float diff = NdotL;
        float spec = (NdotH > 0.0f) ? std::pow(NdotH, properties.smoothness) : 0.0f;

        // Blinn-Phong shading
        Vec3 ambient = Vec3(properties.ambient);
        Vec3 diffuse = Vec3(properties.diffuse) * diff;
        Vec3 specular = Vec3(properties.specular) * spec;

        Vec3 fragColor = (ambient + diffuse + specular) * light.color;

        // Clamp and convert to color
        return sf::Color(
            static_cast<sf::Uint8>(std::min(fragColor.x * 255.0f, 255.0f)),
            static_cast<sf::Uint8>(std::min(fragColor.y * 255.0f, 255.0f)),
            static_cast<sf::Uint8>(std::min(fragColor.z * 255.0f, 255.0f))
        );
    }
};

class ShaderToon : public Shader
{
private:
    ToonProperties properties;

public:
    ShaderProperties* GetProperties() { return &properties; }

    VertexOutput VertexShader(const Vec3& position, const Vec3& normal, const Camera& camera) override
    {
        VertexOutput output;
        Vec3 viewPos = TransformPositionToViewSpace(position, camera);
        output.positionVS = viewPos;
        output.normalVS = TransformNormalToViewSpace(normal, position, camera);
        output.positionCS = TransformPositionToClipSpace(position, camera);
        return output;
    }

    sf::Color FragmentShader(const VertexOutput& vertexData,
        const Light& light,
        const Camera& camera) override
    {

        Vec3 normal = vertexData.normalVS.normalize();
        Vec3 lightDirVS = (TransformPositionToViewSpace(light.position, camera) - vertexData.positionVS).normalize();
        Vec3 viewDirVS = (camera.position - vertexData.positionVS).normalize();

        // Calculate base toon shading
        float NdotL = std::max(normal.dot(lightDirVS), 0.0f);
        float halfSoftness = properties.softness * 0.5f;
        float toonDiffuse = smoothstep(0.5f - halfSoftness, 0.5f + halfSoftness, NdotL);

        // Calculate basic rim factor
        float NdotV = std::max(normal.dot(viewDirVS), 0.0f);
        float rimFactor = 1.0f - NdotV;

        // Calculate light-direction influence
        float directionMask;
        if (properties.rimDirection > 0)
        {
            // Light-side rim: stronger when facing light
            directionMask = NdotL;
        }
        else if (properties.rimDirection < 0)
        {
            // Dark-side rim: stronger when facing away from light
            directionMask = 1.0f - NdotL;
        }
        else
        {
            // No direction preference
            directionMask = 1.0f;
        }

        // Apply direction mask with strength based on rimDirection value
        float directionStrength = std::abs(properties.rimDirection);
        float finalRimFactor = rimFactor * (directionMask * directionStrength + (1.0f - directionStrength));

        // Apply rim width and softness
        float rimThreshold = 1.0f - properties.rimWidth;
        float rimAmount = smoothstep(rimThreshold - properties.rimSoftness,
            rimThreshold + properties.rimSoftness,
            finalRimFactor);

        // Calculate base color (ambient + diffuse)
        Vec3 baseColor = properties.ambient + properties.baseColor * toonDiffuse;

        // Blend rim light over base color
        // Use rim amount as blend factor
        Vec3 fragColor = baseColor * (1.0f - rimAmount) + properties.rimColor * rimAmount;

        // Apply light color
        fragColor = fragColor * light.color;

        // Clamp and convert to color
        return sf::Color(
            static_cast<sf::Uint8>(std::min(fragColor.x * 255.0f, 255.0f)),
            static_cast<sf::Uint8>(std::min(fragColor.y * 255.0f, 255.0f)),
            static_cast<sf::Uint8>(std::min(fragColor.z * 255.0f, 255.0f))
        );
    }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>Slider Control>>>>>>>>>>>>>>>>>>>>>>>>>//

class Slider
{
private:
    float value;
    float minValue;
    float maxValue;

    sf::RectangleShape bar;
    sf::RectangleShape handle; // Draggable handle
    sf::Text titleText;
    sf::Text valueText;
    sf::Font font;

    bool isDragging;
    sf::Vector2f position;
    float width;
    float height;

    void UpdateHandlePosition()
    {
        float normalizedValue = (value - minValue) / (maxValue - minValue);
        handle.setPosition(position.x + normalizedValue * width, position.y);
    }

    void UpdateValueText()
    {
        valueText.setString(std::to_string(value));
        valueText.setPosition(position.x + width + 10, position.y - 5);
    }

    void UpdateSlider()
    {
        UpdateHandlePosition();
        UpdateValueText();
    }

public:
    Slider(const sf::Font& font, const std::string& title,
        float min, float max, float defaultValue,
        const sf::Vector2f& pos, float w = 150.0f, float h = 10.0f)
        : minValue(min), maxValue(max), value(defaultValue),
        position(pos), width(w), height(h), isDragging(false)
    {
        // Initialize bar
        bar.setSize(sf::Vector2f(width, height));
        bar.setPosition(position);
        bar.setFillColor(sf::Color(100, 100, 100));

        // Initialize handle
        handle.setSize(sf::Vector2f(20, height + 10));
        handle.setFillColor(sf::Color::White);
        handle.setOrigin((handle.getSize().x) / 2.0f, (height / 2)); //Set origin to the center of the handle

        // Initialize title text
        this->font = font;
        titleText.setFont(font);
        titleText.setString(title);
        titleText.setCharacterSize(16);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(position.x, position.y - 25);

        // Initialize value text
        valueText.setFont(font);
        valueText.setCharacterSize(16);
        valueText.setFillColor(sf::Color::White);

        UpdateSlider();
    }

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect handleBounds = handle.getGlobalBounds();

        switch (event.type)
        {
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (handleBounds.contains((float)mousePos.x, (float)mousePos.y))
                {
                    isDragging = true;
                }
            }
            break;

        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                isDragging = false;
            }
            break;

        case sf::Event::MouseMoved:
            if (isDragging)
            {
                float normalizedX = (mousePos.x - position.x) / (width);
                normalizedX = std::max(0.0f, std::min(1.0f, normalizedX));
                value = minValue + normalizedX * (maxValue - minValue);
                UpdateSlider();
            }
            break;
        }
    }

    void Draw(sf::RenderWindow& window)
    {
        window.draw(bar);
        window.draw(handle);
        window.draw(titleText);
        window.draw(valueText);
    }

    float GetValue() const
    {
        return value;
    }

    void SetValue(float newValue)
    {
        value = std::max(minValue, std::min(maxValue, newValue));
        UpdateSlider();
    }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>Geometry Objects>>>>>>>>>>>>>>>>>>>>>>>>>//

class RenderableObject
{
public:
    virtual const std::vector<Vec3>& GetVertices() const = 0;
    virtual const std::vector<unsigned int>& GetIndices() const = 0;
    virtual float GetRadius() const = 0;
    virtual Vec3 GetPosition() const = 0;
};

class Sphere : public RenderableObject
{
private:
    std::vector<Vec3> vertices;
    std::vector<unsigned int> indices;
    float radius;
    unsigned int segments;
    Vec3 position; // Position of the sphere

    void GenerateSphere()
    {
        vertices.clear();
        indices.clear();

        // Generate vertices
        for (unsigned int y = 0; y <= segments; ++y)
        {
            float phi = static_cast<float>(y) * M_PI / segments;

            for (unsigned int x = 0; x <= segments; ++x)
            {
                float theta = static_cast<float>(x) * 2.0f * M_PI / segments;

                float xPos = radius * std::sin(phi) * std::cos(theta);
                float yPos = radius * std::cos(phi);
                float zPos = radius * std::sin(phi) * std::sin(theta);

                vertices.emplace_back(xPos + position.x, yPos + position.y, zPos + position.z);
            }
        }

        // Generate indices
        for (unsigned int y = 0; y < segments; ++y)
        {
            for (unsigned int x = 0; x < segments; ++x)
            {
                unsigned int current = y * (segments + 1) + x;
                unsigned int next = current + 1;
                unsigned int bottom = (y + 1) * (segments + 1) + x;
                unsigned int bottomNext = bottom + 1;

                // Check if indices are within bounds
                if (current < vertices.size() &&
                    next < vertices.size() &&
                    bottom < vertices.size() &&
                    bottomNext < vertices.size())
                {
                    // First triangle
                    indices.push_back(current);
                    indices.push_back(bottom);
                    indices.push_back(next);

                    // Second triangle
                    indices.push_back(next);
                    indices.push_back(bottom);
                    indices.push_back(bottomNext);
                }
            }
        }
    }

public:
    Sphere(float r, unsigned int seg, Vec3 pos)
        : radius(r), segments(seg), position(pos)
    {
        GenerateSphere();
    }

    const std::vector<Vec3>& GetVertices() const override { return vertices; }
    const std::vector<unsigned int>& GetIndices() const override { return indices; }
    float GetRadius() const override { return radius; }
    Vec3 GetPosition() const override { return position; }
    void SetPosition(Vec3& newPos)
    {
        position = newPos;
    }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>Render Pipeline>>>>>>>>>>>>>>>>>>>>>>>>>//

class RenderPipeline
{
private:
    static constexpr int WIDTH = 1080;
    static constexpr int HEIGHT = 720;

    sf::Image image;
    std::vector<float> depthBuffer;
    Camera camera;
    Light light;
    Vec3 cameraPos;

    std::vector<std::unique_ptr<RenderableObject>> renderObjects;
    std::vector<std::unique_ptr<Shader>> shaders;
    size_t currentShaderIndex = 0;

    void InitializeDepthBuffer()
    {
        depthBuffer.resize(WIDTH * HEIGHT, std::numeric_limits<float>::infinity());
    }

    void ClearBuffers()
    {
        image.create(WIDTH, HEIGHT, sf::Color::Black);
        std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::infinity());
    }

    Vec3 Barycentric(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c)
    {
        Vec3 v0 = b - a;
        Vec3 v1 = c - a;
        Vec3 v2 = p - a;

        float d00 = v0.dot(v0);
        float d01 = v0.dot(v1);
        float d11 = v1.dot(v1);
        float d20 = v2.dot(v0);
        float d21 = v2.dot(v1);

        float denom = d00 * d11 - d01 * d01;

        // Check for degenerate triangle
        if (std::abs(denom) < 1e-6f)
        {
            return Vec3(-1, -1, -1);  // Invalid barycentric coordinates
        }

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        return Vec3(u, v, w);
    }

    void RasterizeTriangle(const Shader::VertexOutput& v0,
        const Shader::VertexOutput& v1,
        const Shader::VertexOutput& v2)
    {
        // Early frustum culling
        if (v0.positionCS.z < -1.0f || v0.positionCS.z > 1.0f ||
            v1.positionCS.z < -1.0f || v1.positionCS.z > 1.0f ||
            v2.positionCS.z < -1.0f || v2.positionCS.z > 1.0f)
        {
            return;
        }

        // Convert to screen space
        Vec3 p0 = Vec3((v0.positionCS.x + 1.0f) * WIDTH * 0.5f,
            (v0.positionCS.y + 1.0f) * HEIGHT * 0.5f,
            v0.positionCS.z);
        Vec3 p1 = Vec3((v1.positionCS.x + 1.0f) * WIDTH * 0.5f,
            (v1.positionCS.y + 1.0f) * HEIGHT * 0.5f,
            v1.positionCS.z);
        Vec3 p2 = Vec3((v2.positionCS.x + 1.0f) * WIDTH * 0.5f,
            (v2.positionCS.y + 1.0f) * HEIGHT * 0.5f,
            v2.positionCS.z);

        // Calculate bounding box with guards against float-to-int conversion issues
        int minX = std::max(0, static_cast<int>(std::floor(std::min({ p0.x, p1.x, p2.x }))));
        int maxX = std::min(WIDTH - 1, static_cast<int>(std::ceil(std::max({ p0.x, p1.x, p2.x }))));
        int minY = std::max(0, static_cast<int>(std::floor(std::min({ p0.y, p1.y, p2.y }))));
        int maxY = std::min(HEIGHT - 1, static_cast<int>(std::ceil(std::max({ p0.y, p1.y, p2.y }))));

        // Skip if triangle is completely outside screen
        if (maxX < minX || maxY < minY) return;

        // Rasterize
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                // Check array bounds before accessing depthBuffer
                if (y * WIDTH + x >= depthBuffer.size()) continue;

                Vec3 p(x + 0.5f, y + 0.5f, 0);
                Vec3 bary = Barycentric(p, p0, p1, p2);

                // Check if point is inside triangle and barycentric coordinates are valid
                if (bary.x >= -0.01f && bary.y >= -0.01f && bary.z >= -0.01f &&
                    bary.x <= 1.01f && bary.y <= 1.01f && bary.z <= 1.01f)
                {
                    // Interpolate Z with perspective correction
                    float z = 1.0f / (bary.x / v0.positionCS.z + bary.y / v1.positionCS.z + bary.z / v2.positionCS.z);

                    // Depth test
                    if (z < depthBuffer[y * WIDTH + x])
                    {
                        depthBuffer[y * WIDTH + x] = z;

                        // Interpolate attributes for fragment shader with perspective correction
                        Vec3 worldPos = (v0.positionVS * bary.x + v1.positionVS * bary.y + v2.positionVS * bary.z);
                        Vec3 normal = (v0.normalVS * bary.x + v1.normalVS * bary.y + v2.normalVS * bary.z).normalize();

                        Shader::VertexOutput interpolated;
                        interpolated.positionVS = worldPos;
                        interpolated.normalVS = normal;

                        sf::Color color = shaders[currentShaderIndex]->FragmentShader(
                            interpolated, light, cameraPos);
                        image.setPixel(x, y, color);
                    }
                }
            }
        }
    }

    void RenderObject(const RenderableObject& object)
    {
        const auto& vertices = object.GetVertices();
        const auto& indices = object.GetIndices();

        // Process triangles
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            if (indices[i] >= vertices.size() ||
                indices[i + 1] >= vertices.size() ||
                indices[i + 2] >= vertices.size())
            {
                continue;  // Skip invalid triangles
            }

            Vec3 v0 = vertices[indices[i]];
            Vec3 v1 = vertices[indices[i + 1]];
            Vec3 v2 = vertices[indices[i + 2]];

            // Calculate normals (simplified - you might want to store these with vertices)
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 normal = Vec3(
                edge1.y * edge2.z - edge1.z * edge2.y,
                edge1.z * edge2.x - edge1.x * edge2.z,
                edge1.x * edge2.y - edge1.y * edge2.x
            ).normalize();

            // Run vertex shader for each vertex using current shader
            auto vOut0 = shaders[currentShaderIndex]->VertexShader(v0, normal, camera);
            auto vOut1 = shaders[currentShaderIndex]->VertexShader(v1, normal, camera);
            auto vOut2 = shaders[currentShaderIndex]->VertexShader(v2, normal, camera);

            // Rasterize the triangle
            RasterizeTriangle(vOut0, vOut1, vOut2);
        }
    }

public:
    RenderPipeline()
    {
        // Initialize rendering components
        image.create(WIDTH, HEIGHT, sf::Color::Black);
        InitializeDepthBuffer();

        // Initialize camera
        camera = Camera(Vec3(0.0f, 0.0f, 10.0f),
            Vec3(0.0f, 0.0f, -1.0f),
            60.0f,
            static_cast<float>(WIDTH) / HEIGHT,
            0.1f,
            100.0f);

        // Initialize light
        light.position = Vec3(-10.0f, 10.0f, 10.0f);
        light.color = Vec3(1.0f, 1.0f, 1.0f);

        // Set camera position
        cameraPos = Vec3(0.0f, 0.0f, 10.0f);

        // Add sphere to scene
        renderObjects.push_back(std::make_unique<Sphere>(3.0f, 64, Vec3(0.0f, 0.0f, 0.0f)));

        // Initialize shaders
        InitializeShaders();

        // Initial render
        Render();
    }

    void InitializeShaders()
    {
        shaders.push_back(std::make_unique<ShaderBlinnPhong>());
        shaders.push_back(std::make_unique<ShaderToon>());
    }

    void Render() {
        ClearBuffers();
        for (const auto& object : renderObjects)
        {
            RenderObject(*object);
        }
    }

    // Getters and setters for properties
    const sf::Image& GetImage() const { return image; }
    ShaderProperties* GetCurrentShaderProperties() { return shaders[currentShaderIndex]->GetProperties(); }
    size_t GetCurrentShaderIndex() const { return currentShaderIndex; }
    void SetCurrentShaderIndex(size_t index) { currentShaderIndex = index % shaders.size(); }

    Camera& GetCamera() { return camera; }
    void SetCamera(const Camera& newCamera) { camera = newCamera; }

    Light& GetLight() { return light; }
    void SetLight(const Light& newLight) { light = newLight; }

    size_t GetWidth() const { return WIDTH; }
    size_t GetHeight() const { return HEIGHT; }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>Main Program Class>>>>>>>>>>>>>>>>>>>>>>>>>//

class MaterialPreviewer
{
private:
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Font font;
    std::vector<Slider> sliders;
    sf::Text shaderNameText;
    RenderPipeline renderPipeline;

    void UpdateShaderUI()
    {
        sliders.clear();

        auto properties = renderPipeline.GetCurrentShaderProperties();

        // Update shader name text
        shaderNameText.setFont(font);
        shaderNameText.setString(properties->GetShaderName());
        shaderNameText.setCharacterSize(20);
        shaderNameText.setFillColor(sf::Color::White);
        shaderNameText.setPosition(20, 20);

        // Create property sliders
        float yPos = 80.0f;
        const auto& sliderProps = properties->GetSliderProperties();

        // Calculate right-side position for light controls
        float rightSideX = renderPipeline.GetWidth() - 250.0f;

        // Create sliders for shader-specific properties
        CreateShaderPropertySliders(properties, sliderProps, yPos);

        // Add light control sliders
        CreateLightControlSliders(rightSideX);
    }

    void CreateShaderPropertySliders(ShaderProperties* properties,
        const std::map<std::string, std::pair<float, float>>& sliderProps,
        float& yPos)
    {
        for (const auto& prop : sliderProps)
        {
            const std::string& propName = prop.first;
            const std::pair<float, float>& propRange = prop.second;
            float initialValue = GetInitialValueForProperty(properties, propName);

            sliders.emplace_back(font, propName, propRange.first, propRange.second,
                initialValue, sf::Vector2f(20, yPos));
            yPos += 40.0f;
        }
    }

    void CreateLightControlSliders(float rightSideX)
    {
        float rightYPos = 80.0f;
        Light& light = renderPipeline.GetLight();

        // Light Position Controls
        sliders.emplace_back(font, "Light Position X", -20.0f, 20.0f, light.position.x,
            sf::Vector2f(rightSideX, rightYPos)); rightYPos += 40.0f;
        sliders.emplace_back(font, "Light Position Y", -20.0f, 20.0f, light.position.y,
            sf::Vector2f(rightSideX, rightYPos)); rightYPos += 40.0f;
        sliders.emplace_back(font, "Light Position Z", -20.0f, 20.0f, light.position.z,
            sf::Vector2f(rightSideX, rightYPos)); rightYPos += 40.0f;

        // Light Color Controls
        sliders.emplace_back(font, "Light Color Red", 0.0f, 1.0f, light.color.x,
            sf::Vector2f(rightSideX, rightYPos)); rightYPos += 40.0f;
        sliders.emplace_back(font, "Light Color Green", 0.0f, 1.0f, light.color.y,
            sf::Vector2f(rightSideX, rightYPos)); rightYPos += 40.0f;
        sliders.emplace_back(font, "Light Color Blue", 0.0f, 1.0f, light.color.z,
            sf::Vector2f(rightSideX, rightYPos));
    }

    float GetBlinnPhongInitialValue(BlinnPhongProperties* properties, const std::string& propName)
    {
        switch (GetPropertyEnum(propName))
        {
        case BlinnPhongProperty::AmbientX_Red: return properties->ambient.x;
        case BlinnPhongProperty::AmbientY_Green: return properties->ambient.y;
        case BlinnPhongProperty::AmbientZ_Blue: return properties->ambient.z;
        case BlinnPhongProperty::DiffuseX_Red: return properties->diffuse.x;
        case BlinnPhongProperty::DiffuseY_Green: return properties->diffuse.y;
        case BlinnPhongProperty::DiffuseZ_Blue: return properties->diffuse.z;
        case BlinnPhongProperty::SpecularX_Red: return properties->specular.x;
        case BlinnPhongProperty::SpecularY_Green: return properties->specular.y;
        case BlinnPhongProperty::SpecularZ_Blue: return properties->specular.z;
        case BlinnPhongProperty::Smoothness: return properties->smoothness;
        }
        return 0.0f;
    }

    float GetToonInitialValue(ToonProperties* properties, const std::string& propName)
    {
        switch (GetToonPropertyEnum(propName))
        {
        case ToonProperty::AmbientX_Red: return properties->ambient.x;
        case ToonProperty::AmbientY_Green: return properties->ambient.y;
        case ToonProperty::AmbientZ_Blue: return properties->ambient.z;
        case ToonProperty::BaseColorX_Red: return properties->baseColor.x;
        case ToonProperty::BaseColorY_Green: return properties->baseColor.y;
        case ToonProperty::BaseColorZ_Blue: return properties->baseColor.z;
        case ToonProperty::DiffuseSoftness: return properties->softness;
        case ToonProperty::RimColorX_Red: return properties->rimColor.x;
        case ToonProperty::RimColorY_Green: return properties->rimColor.y;
        case ToonProperty::RimColorZ_Blue: return properties->rimColor.z;
        case ToonProperty::RimWidth: return properties->rimWidth;
        case ToonProperty::RimSoftness: return properties->rimSoftness;
        case ToonProperty::RimDirection: return properties->rimDirection;
        }
        return 0.0f;
    }

    float GetInitialValueForProperty(ShaderProperties* properties, const std::string& propName)
    {
        if (auto blinnPhong = dynamic_cast<BlinnPhongProperties*>(properties))
        {
            return GetBlinnPhongInitialValue(blinnPhong, propName);
        }
        else if (auto toon = dynamic_cast<ToonProperties*>(properties))
        {
            return GetToonInitialValue(toon, propName);
        }
        return 0.0f;
    }

    void UpdateShaderProperties(ShaderProperties* properties,
        const std::map<std::string, std::pair<float, float>>& sliderProps)
    {
        size_t i = 0;
        for (const auto& prop : sliderProps)
        {
            const std::string& propName = prop.first;
            float value = sliders[i].GetValue();

            if (auto blinnPhong = dynamic_cast<BlinnPhongProperties*>(properties))
            {
                switch (GetPropertyEnum(propName)) {
                case BlinnPhongProperty::AmbientX_Red: blinnPhong->ambient.x = value; break;
                case BlinnPhongProperty::AmbientY_Green: blinnPhong->ambient.y = value; break;
                case BlinnPhongProperty::AmbientZ_Blue: blinnPhong->ambient.z = value; break;
                case BlinnPhongProperty::DiffuseX_Red: blinnPhong->diffuse.x = value; break;
                case BlinnPhongProperty::DiffuseY_Green: blinnPhong->diffuse.y = value; break;
                case BlinnPhongProperty::DiffuseZ_Blue: blinnPhong->diffuse.z = value; break;
                case BlinnPhongProperty::SpecularX_Red: blinnPhong->specular.x = value; break;
                case BlinnPhongProperty::SpecularY_Green: blinnPhong->specular.y = value; break;
                case BlinnPhongProperty::SpecularZ_Blue: blinnPhong->specular.z = value; break;
                case BlinnPhongProperty::Smoothness: blinnPhong->smoothness = value; break;
                }
            }
            else if (auto toon = dynamic_cast<ToonProperties*>(properties))
            {
                switch (GetToonPropertyEnum(propName))
                {
                case ToonProperty::AmbientX_Red: toon->ambient.x = value; break;
                case ToonProperty::AmbientY_Green: toon->ambient.y = value; break;
                case ToonProperty::AmbientZ_Blue: toon->ambient.z = value; break;
                case ToonProperty::BaseColorX_Red: toon->baseColor.x = value; break;
                case ToonProperty::BaseColorY_Green: toon->baseColor.y = value; break;
                case ToonProperty::BaseColorZ_Blue: toon->baseColor.z = value; break;
                case ToonProperty::DiffuseSoftness: toon->softness = value; break;
                case ToonProperty::RimColorX_Red: toon->rimColor.x = value; break;
                case ToonProperty::RimColorY_Green: toon->rimColor.y = value; break;
                case ToonProperty::RimColorZ_Blue: toon->rimColor.z = value; break;
                case ToonProperty::RimWidth: toon->rimWidth = value; break;
                case ToonProperty::RimSoftness: toon->rimSoftness = value; break;
                case ToonProperty::RimDirection: toon->rimDirection = value; break;
                }
            }
            i++;
        }
    }

    void UpdateLightProperties(size_t shaderPropCount)
    {
        Light& light = renderPipeline.GetLight();

        // Update light position
        light.position.x = sliders[shaderPropCount].GetValue();
        light.position.y = sliders[shaderPropCount + 1].GetValue();
        light.position.z = sliders[shaderPropCount + 2].GetValue();

        // Update light color
        light.color.x = sliders[shaderPropCount + 3].GetValue();
        light.color.y = sliders[shaderPropCount + 4].GetValue();
        light.color.z = sliders[shaderPropCount + 5].GetValue();

        renderPipeline.SetLight(light);
    }

    void UpdateProperties()
    {
        auto properties = renderPipeline.GetCurrentShaderProperties();
        auto sliderProps = properties->GetSliderProperties();

        // Update shader-specific properties
        UpdateShaderProperties(properties, sliderProps);

        // Update light properties
        UpdateLightProperties(sliderProps.size());

        // Render with updated properties
        renderPipeline.Render();
        texture.loadFromImage(renderPipeline.GetImage());
    }

    void HandleEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::C) {
                    size_t newIndex = renderPipeline.GetCurrentShaderIndex() + 1;
                    renderPipeline.SetCurrentShaderIndex(newIndex);
                    UpdateShaderUI();
                }
            }

            for (auto& slider : sliders)
            {
                slider.HandleEvent(event, window);
            }
        }
    }

public:
    MaterialPreviewer()
        : window(sf::VideoMode(renderPipeline.GetWidth(), renderPipeline.GetHeight()),
            "Material Previewer")
    {
        if (!font.loadFromFile("arial.ttf"))
        {
            std::cerr << "Error loading font\n";
        }

        texture.loadFromImage(renderPipeline.GetImage());
        sprite.setTexture(texture);

        UpdateShaderUI();
    }

    void Run()
    {
        while (window.isOpen())
        {
            HandleEvents();
            UpdateProperties();

            window.clear();
            window.draw(sprite);
            window.draw(shaderNameText);
            for (auto& slider : sliders)
            {
                slider.Draw(window);
            }
            window.display();
        }
    }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>Main Program>>>>>>>>>>>>>>>>>>>>>>>>>//

int main()
{
    MaterialPreviewer previewer;
    previewer.Run();
    return 0;
}