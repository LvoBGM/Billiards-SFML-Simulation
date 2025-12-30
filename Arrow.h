#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
class Arrow : public sf::Drawable, public sf::Transformable
{
public:
    Arrow(sf::Vector2f origin, sf::Vector2f end, sf::Color color);

    void setVector(const sf::Vector2f& v);
    void setColor(const sf::Color& color);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }

    sf::Vector2f m_vector;
    float m_widthHeightRatio = 1.f / 6.f;
    float m_headTailRatio = 1.f / 3.f;
    sf::Color m_color;

    sf::VertexArray m_vertices;
};

