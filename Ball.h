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

    // Static methods
    static void checkForCollisions(const float& dt);
    static std::pair<sf::Vector2f, sf::Vector2f> ballCollision(const std::unique_ptr<Ball>& hitterBall, const std::unique_ptr<Ball>& hitBall);
    static void updatePositions();

    void calculateNextPosition(const float& dt, const sf::Vector2u& windowSize);

    void calcFuturePos(const float& dt, const sf::Vector2u& windowSize);

    // Getters
    sf::Vector2f getNextForce();
    sf::Vector2f getFuturePosition();

    // Setters
    void setFuturePosition(const sf::Vector2f& v);
    void setNextForce(const sf::Vector2f v);

private:
    // Private members
    float m_dt = 0.f;
    sf::Vector2u m_windowSize;
    sf::Vector2f m_nextForce;
    sf::Vector2f m_futurePosition;

    void wallCheck(const sf::Vector2f& position);
};

