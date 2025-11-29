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
    sf::Vector2f calculateFinalForce(const float& dt, const sf::Vector2u& windowSize);
    void setInsideBall(bool b);
    void movingCollision(const float& dt, const sf::Vector2u& windowSize, Ball* hitterBall);
private:
    std::vector<sf::Vector2f> m_forces;
    bool m_insideBall = false;
    bool m_finalForceCalculated = false;

    float m_dt = 0.f;
    sf::Vector2u m_windowSize;
    sf::Vector2f m_calculatedFinalForce;

    sf::Vector2f _ballCollision(Ball* hitBall, const sf::Vector2f& finalForce, bool isHitterBall = true);
    void _outOfBoundCheck(const sf::Vector2f& position, const float& radius);
};

