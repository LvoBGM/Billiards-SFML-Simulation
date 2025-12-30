#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
class Arrow : public sf::Drawable, public sf::Transformable
{
public:
    Arrow();

    void setVector(const sf::Vector2f& v);
    void setColor(const sf::Color& color);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }

    sf::Vector2f m_vector;
    sf::Color m_color;
    float m_thickness;

    sf::VertexArray m_vertices;
};

