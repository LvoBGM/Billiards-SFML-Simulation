#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Ball.h"

float frictionCoefficient = 0.25f;
float minimumFriction = 0.1f;

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1000, 800 }), "Billiards");
    
    Ball::makeBall(50, { 200, 200 }, sf::Color::Red);

    
    // Useful parameters
    float poolCuePower = 2;

    bool paused = false;
    bool rightMousePressed = false;
    Ball* selectedBall = nullptr;
    sf::Vector2f mouseForceVector{};
    sf::Clock clock;
    while (window.isOpen()) {

        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (keyPressed->button == sf::Mouse::Button::Left && !paused) {
                    for (const std::unique_ptr<Ball>& ball : Ball::s_Balls) {
                        sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                        sf::Vector2f mouseToBallVector = ball->getPosition() - localMousePosition;

                        float distance = std::sqrt(mouseToBallVector.x * mouseToBallVector.x + mouseToBallVector.y * mouseToBallVector.y);

                        if (distance < ball->getRadius()) {
                            mouseForceVector = ball->getPosition();
                            selectedBall = ball.get();
                            paused = true;
                            break;
                        }
                    }
                }
                else if (keyPressed->button == sf::Mouse::Button::Right && !paused) {
                    sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                    Ball::makeBall(50, localMousePosition, sf::Color::Green);
                }
                    
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (keyReleased->button == sf::Mouse::Button::Left) {
                    if (paused) {
                        sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                        mouseForceVector = mouseForceVector - localMousePosition;
                        selectedBall->addForce(mouseForceVector * poolCuePower);

                        selectedBall = nullptr;

                        paused = false;
                    }
                }
                else if (keyReleased->button == sf::Mouse::Button::Right) {
                    rightMousePressed = false;
                }
            }
        }
        if (!paused) {
            // Update physics
            for (const std::unique_ptr<Ball>& ball : Ball::s_Balls) {
                ball->updatePosition(deltaTime, window.getSize());
            }
        }
        
        window.clear();
        for (const std::unique_ptr<Ball>& ball : Ball::s_Balls) {
            window.draw(*ball);
        }
        window.display();
    }
}