#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
class Ball :
    public sf::CircleShape
{
public:
    using CircleShape::CircleShape;
    inline static std::vector<std::unique_ptr<Ball>> s_Balls;

    // Factory Constructor
    static Ball& makeBall(float size, sf::Vector2f position, sf::Color color);

    //static void checkForCollisions();

    void addForce(sf::Vector2f force);
    void calculateNextPosition(const float& dt, const sf::Vector2u& windowSize);

    void calcFuturePos(const float& dt, const sf::Vector2u& windowSize);
    void movingCollision(const float& dt, const sf::Vector2u& windowSize, Ball* hitterBall);

private:
    // Private members
    std::vector<sf::Vector2f> m_forces;
    
    float m_dt = 0.f;
    sf::Vector2u m_windowSize;
    sf::Vector2f m_nextForce;
    sf::Vector2f m_futurePosition;


    sf::Vector2f _ballCollision(Ball* hitBall, const sf::Vector2f& finalForce, bool isHitterBall = true);
    void wallCheck(const sf::Vector2f& position);
};

