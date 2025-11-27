#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
class Ball :
    public sf::CircleShape
{
public:
    using CircleShape::CircleShape;

    static std::vector<Ball> s_Balls;

    void AddForce(sf::Vector2f force);
    void UpdatePosition(const float& dt, const sf::Vector2u& windowSize);
private:
    std::vector<sf::Vector2f> m_forces;
};

