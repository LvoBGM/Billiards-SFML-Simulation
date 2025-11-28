#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
class Ball :
    public sf::CircleShape
{
public:
    using CircleShape::CircleShape;
    inline static std::vector<Ball*> s_Balls;

    Ball(float radius, std::size_t pointCount = 30)
        : sf::CircleShape(radius, pointCount)
    {
        s_Balls.push_back(this);
    }

    void addForce(sf::Vector2f force);
    void updatePosition(const float& dt, const sf::Vector2u& windowSize);
    void setInsideBall(bool b);
private:
    std::vector<sf::Vector2f> m_forces;
    bool m_insideBall;

    void _ballCollision(Ball* hitBall, sf::Vector2f& finalForce);
};

