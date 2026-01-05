#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <set>
class Arrow : public sf::Drawable, public sf::Transformable
{
public:
    inline static std::set<Arrow*> s_ArrowsToDraw;

    Arrow();
    Arrow(sf::Vector2f origin, sf::Vector2f end, sf::Color color);
    ~Arrow();

    void toBeDrawn(bool b);

    // Setters
    void setVector(const sf::Vector2f& v);
    void setColor(const sf::Color& color);

    // Getters
    sf::Vector2f getVector();

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }

    void UpdateGeometry();

    sf::Vector2f m_vector;
    float m_widthHeightRatio = 1.f / 6.f;
    float m_headTailRatio = 1.f / 3.f;
    sf::Color m_color;

    sf::VertexArray m_vertices;
};

