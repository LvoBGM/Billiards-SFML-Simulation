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
    void movingCollision(Ball* hitterBall, sf::Vector2f& addedForce);
private:
    std::vector<sf::Vector2f> m_forces;
    bool m_insideBall;

    sf::Vector2f _calculateFinalForce(const float& dt, const sf::Vector2u& windowSize);
    sf::Vector2f _ballCollision(Ball* hitBall, const sf::Vector2f& finalForce);
    void _outOfBoundCheck(const sf::Vector2f& position, const float& radius, const sf::Vector2u windowSize);
};

