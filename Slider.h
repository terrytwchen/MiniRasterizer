#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>

class Slider
{
private:
    float _value;
    float _minValue;
    float _maxValue;

    sf::RectangleShape _bar;
    sf::RectangleShape _handle;
    sf::Text _titleText;
    sf::Text _valueText;
    sf::Font _font;

    bool _isDragging;
    sf::Vector2f _position;
    float _width;
    float _height;

    void _UpdateHandlePosition()
    {
        float normalizedValue = (_value - _minValue) / (_maxValue - _minValue);
        _handle.setPosition(_position.x + normalizedValue * _width, _position.y);
    }

    void _UpdateValueText()
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f", _value);
        _valueText.setString(buffer);
        _valueText.setPosition(_position.x + _width + 10, _position.y - 5);
    }

    void _UpdateSlider()
    {
        _UpdateHandlePosition();
        _UpdateValueText();
    }

public:
    Slider(const sf::Font& font, const std::string& title,
        float min, float max, float defaultValue,
        const sf::Vector2f& pos, float w = 150.0f, float h = 10.0f)
        : _minValue(min), _maxValue(max), _value(defaultValue),
        _position(pos), _width(w), _height(h), _isDragging(false), _font(font)
    {
        _bar.setSize(sf::Vector2f(_width, _height));
        _bar.setPosition(_position);
        _bar.setFillColor(sf::Color(100, 100, 100));

        _handle.setSize(sf::Vector2f(20, _height + 10));
        _handle.setFillColor(sf::Color::White);
        _handle.setOrigin((_handle.getSize().x) / 2.0f, (_height / 2));

        _titleText.setFont(_font);
        _titleText.setString(title);
        _titleText.setCharacterSize(16);
        _titleText.setFillColor(sf::Color::White);
        _titleText.setPosition(_position.x, _position.y - 25);

        _valueText.setFont(_font);
        _valueText.setCharacterSize(16);
        _valueText.setFillColor(sf::Color::White);

        _UpdateSlider();
    }

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect handleBounds = _handle.getGlobalBounds();

        switch (event.type)
        {
        case sf::Event::MouseButtonPressed:
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (handleBounds.contains((float)mousePos.x, (float)mousePos.y))
                {
                    _isDragging = true;
                }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                _isDragging = false;
            }
            break;
        }
        case sf::Event::MouseMoved:
        {
            if (_isDragging)
            {
                float normalizedX = (mousePos.x - _position.x) / (_width);
                normalizedX = std::max(0.0f, std::min(1.0f, normalizedX));
                _value = _minValue + normalizedX * (_maxValue - _minValue);
                _UpdateSlider();
            }
            break;
        }
        default:
            break;
        }
    }

    void Draw(sf::RenderWindow& window)
    {
        window.draw(_bar);
        window.draw(_handle);
        window.draw(_titleText);
        window.draw(_valueText);
    }

    float GetValue() const
    {
        return _value;
    }

    void SetValue(float newValue)
    {
        _value = std::max(_minValue, std::min(_maxValue, newValue));
        _UpdateSlider();
    }
};