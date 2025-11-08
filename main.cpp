#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>
#include <map>
#include <utility>
#include <algorithm>

#include "Vec3.h"
#include "Camera.h"
#include "Light.h"
#include "RenderPipeline.h"
#include "RenderableObject.h"
#include "Material.h"
#include "ShaderBlinnPhong.h"
#include "ShaderToon.h"
#include "BlinnPhongProperties.h"
#include "ToonProperties.h"
#include "MeshGenerator.h"
#include "PropertyEnums.h"
#include "Slider.h"

static constexpr int SCREEN_WIDTH = 1080;
static constexpr int SCREEN_HEIGHT = 720;


class MaterialPreviewer
{
private:
    sf::RenderWindow _window;
    RenderPipeline _pipeline;
    sf::Texture _texture;
    sf::Sprite _sprite;
    sf::Image _image; // Pipeline result will be export to SFML image

    Camera _camera;
    Light _light;

    std::vector<std::shared_ptr<RenderableObject>> _scene;

    std::vector<std::shared_ptr<IShader>> _availableShaders;
    std::vector<std::shared_ptr<Material>> _availableMaterials;
    int _currentMaterialIndex = 0;

    // UI
    sf::Font _font;
    std::vector<std::unique_ptr<Slider>> _sliders;
    sf::Text _shaderNameText;
    sf::Text _hintText;

    void _UpdateShaderUI()
    {
        _sliders.clear();

        Material* currentMat = _availableMaterials[_currentMaterialIndex].get();
        ShaderProperties* props = currentMat->GetProperties();

        // Hint text update
        _hintText.setFont(_font);
        _hintText.setString("Press C for Changing Shader");
        _hintText.setCharacterSize(35);
        _hintText.setFillColor(sf::Color(50, 50, 50));
        sf::FloatRect hintTextBound = _hintText.getLocalBounds();
        _hintText.setOrigin(hintTextBound.width / 2.0f, hintTextBound.height / 2.0f);
        _hintText.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 75);

        // Shader name update
        _shaderNameText.setFont(_font);
        _shaderNameText.setString(props->GetShaderName());
        _shaderNameText.setCharacterSize(20);
        _shaderNameText.setFillColor(sf::Color::White);
        _shaderNameText.setPosition(20, 20);

        // Create sliders for properties
        float yPos = 80.0f;
        const auto& sliderProps = props->GetSliderProperties();
        float rightSideX = SCREEN_WIDTH - 250.0f;
        _CreateShaderPropertySliders(props, sliderProps, yPos);
        _CreateLightControlSliders(rightSideX);
    }

    void _CreateShaderPropertySliders(ShaderProperties* properties,
        const std::map<std::string, std::pair<float, float>>& sliderProps,
        float& yPos)
    {
        for (const auto& prop : sliderProps)
        {
            const std::string& propName = prop.first;
            const std::pair<float, float>& propRange = prop.second;
            float initialValue = _GetInitialValueForProperty(properties, propName);

            _sliders.emplace_back(std::make_unique<Slider>(_font, propName, propRange.first, propRange.second,
                initialValue, sf::Vector2f(20, yPos)));
            yPos += 40.0f;
        }
    }

    void _CreateLightControlSliders(float rightSideX)
    {
        float rightYPos = 80.0f;

        _sliders.emplace_back(std::make_unique<Slider>(_font, "Light Position X", -20.0f, 20.0f, _light.position.x,
            sf::Vector2f(rightSideX, rightYPos))); rightYPos += 40.0f;
        _sliders.emplace_back(std::make_unique<Slider>(_font, "Light Position Y", -20.0f, 20.0f, _light.position.y,
            sf::Vector2f(rightSideX, rightYPos))); rightYPos += 40.0f;
        _sliders.emplace_back(std::make_unique<Slider>(_font, "Light Position Z", -20.0f, 20.0f, _light.position.z,
            sf::Vector2f(rightSideX, rightYPos))); rightYPos += 40.0f;

        _sliders.emplace_back(std::make_unique<Slider>(_font, "Light Color Red", 0.0f, 1.0f, _light.color.x,
            sf::Vector2f(rightSideX, rightYPos))); rightYPos += 40.0f;
        _sliders.emplace_back(std::make_unique<Slider>(_font, "Light Color Green", 0.0f, 1.0f, _light.color.y,
            sf::Vector2f(rightSideX, rightYPos))); rightYPos += 40.0f;
        _sliders.emplace_back(std::make_unique<Slider>(_font, "Light Color Blue", 0.0f, 1.0f, _light.color.z,
            sf::Vector2f(rightSideX, rightYPos)));
    }

    float _GetInitialValueForProperty(ShaderProperties* properties, const std::string& propName)
    {
        if (auto blinnPhong = dynamic_cast<BlinnPhongProperties*>(properties))
        {
            return _GetBlinnPhongInitialValue(blinnPhong, propName);
        }
        else if (auto toon = dynamic_cast<ToonProperties*>(properties))
        {
            return _GetToonInitialValue(toon, propName);
        }
        return 0.0f;
    }

    float _GetBlinnPhongInitialValue(BlinnPhongProperties* properties, const std::string& propName)
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

    float _GetToonInitialValue(ToonProperties* properties, const std::string& propName)
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

    void _UpdateShaderProperties(ShaderProperties* properties,
        const std::map<std::string, std::pair<float, float>>& sliderProps)
    {
        size_t i = 0;
        for (const auto& prop : sliderProps)
        {
            const std::string& propName = prop.first;
            float value = _sliders[i]->GetValue();

            if (auto blinnPhong = dynamic_cast<BlinnPhongProperties*>(properties))
            {
                switch (GetPropertyEnum(propName))
                {
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

    void _UpdateLightProperties(size_t shaderPropCount)
    {
        // Get light data from UI
        _light.position.x = _sliders[shaderPropCount]->GetValue();
        _light.position.y = _sliders[shaderPropCount + 1]->GetValue();
        _light.position.z = _sliders[shaderPropCount + 2]->GetValue();
        _light.color.x = _sliders[shaderPropCount + 3]->GetValue();
        _light.color.y = _sliders[shaderPropCount + 4]->GetValue();
        _light.color.z = _sliders[shaderPropCount + 5]->GetValue();

        // Sync light data to Pipeline
        _pipeline.SetLight(_light);
    }


public:
    MaterialPreviewer()
        : _window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "MiniRasterizer (Refactored)"),
        _pipeline(SCREEN_WIDTH, SCREEN_HEIGHT)
    {
        // Create image buffers
        _image.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
        _texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
        _sprite.setTexture(_texture);

        // Create global uniforms
        _camera = Camera(
            Vec3(0.0f, 0.0f, 10.0f),
            Vec3(0.0f, 0.0f, -1.0f),
            60.0f,
            (float)SCREEN_WIDTH / SCREEN_HEIGHT
        );
        _light = Light{ Vec3(-10.0f, 10.0f, 10.0f), Vec3(1.0f, 1.0f, 1.0f) };

        // Set uniforms in the pipeline
        _pipeline.SetCamera(_camera);
        _pipeline.SetLight(_light);

        // Load UI resources
        if (!_font.loadFromFile("arial.ttf"))
        {
            std::cerr << "Error loading font\n";
            throw std::runtime_error("Failed to load font");
        }

        // Create shaders
        _availableShaders.push_back(std::make_shared<ShaderBlinnPhong>());
        _availableShaders.push_back(std::make_shared<ShaderToon>());

        // Create materials (one for each shader)
        _availableMaterials.push_back(std::make_shared<Material>(_availableShaders[0]));
        _availableMaterials.push_back(std::make_shared<Material>(_availableShaders[1]));

        // Create mesh (geometry)
        auto sphereMesh = MeshGenerator::CreateSphere(3.0f, 64, Vec3(0, 0, 0));

        // Create scene object (using the first material by default)
        auto sphereObject = std::make_shared<RenderableObject>(
            sphereMesh,
            _availableMaterials[0],
            Vec3(0, 0, 0)
        );
        _scene.push_back(sphereObject);

        // Initial UI setup
        _UpdateShaderUI();
    }

    void Run()
    {
        while (_window.isOpen())
        {
            HandleEvents();
            Update();
            Render();
        }
    }

    void HandleEvents()
    {
        sf::Event event;
        while (_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                _window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::C)
                {
                    // Switch shader
                    _currentMaterialIndex = (_currentMaterialIndex + 1) % _availableMaterials.size();
                    _scene[0]->SetMaterial(_availableMaterials[_currentMaterialIndex]);
                    _UpdateShaderUI();
                }
            }

            for (auto& slider : _sliders)
            {
                slider->HandleEvent(event, _window);
            }
        }
    }

    void Update()
    {
        // Update properties from UI
        Material* currentMat = _availableMaterials[_currentMaterialIndex].get();
        ShaderProperties* props = currentMat->GetProperties();
        auto sliderPropsMap = props->GetSliderProperties();

        _UpdateShaderProperties(props, sliderPropsMap);
        _UpdateLightProperties(sliderPropsMap.size());
    }

    void Render()
    {
        // ------------------------------------
        // Bind/Draw Render Loop
        // ------------------------------------

        // Clear the pipeline's internal buffers
        _pipeline.ClearBuffers();

        // Iterate through the scene
        for (const auto& obj : _scene)
        {
            // Bind the material
            _pipeline.BindMaterial(obj->GetMaterial().get());

            // Execute the draw call
            _pipeline.Draw(
                *(obj->GetMesh()),
                obj->GetPosition()
            );
        }

        // Blit the pipeline's Vec3 buffer to the SFML Image
        const auto& colorBuffer = _pipeline.GetFinalColorBuffer();
        for (int y = 0; y < _pipeline.GetHeight(); ++y)
        {
            for (int x = 0; x < _pipeline.GetWidth(); ++x)
            {
                const Vec3& color = colorBuffer[y * _pipeline.GetWidth() + x];
                // Simple tonemapping (Clamp)
                sf::Uint8 r = static_cast<sf::Uint8>(std::min(color.x, 1.0f) * 255.0f);
                sf::Uint8 g = static_cast<sf::Uint8>(std::min(color.y, 1.0f) * 255.0f);
                sf::Uint8 b = static_cast<sf::Uint8>(std::min(color.z, 1.0f) * 255.0f);
                _image.setPixel(x, y, sf::Color(r, g, b));
            }
        }

        // Display to the window
        _texture.update(_image);
        _window.clear(sf::Color::Black);
        _window.draw(_sprite);

        // Draw the UI
        _window.draw(_shaderNameText);
        _window.draw(_hintText);
        for (auto& slider : _sliders)
        {
            slider->Draw(_window);
        }

        _window.display();
    }
};

int main()
{
    try
    {
        MaterialPreviewer previewer;
        previewer.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}