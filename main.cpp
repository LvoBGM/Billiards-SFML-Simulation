#include <iostream>
#include <SFML/Graphics.hpp>
#include "Ball.h"

float frictionCoefficient = 0.25f;
float minimumFriction = 0.1f;

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1000, 800 }), "Billiards");
    
    Ball testBall{ 50, 50 };
    testBall.setPosition({200, 200});
    testBall.setFillColor(sf::Color::Red);
    testBall.setOrigin({testBall.getRadius(), testBall.getRadius()});

    Ball testBall2{ 50, 50 };
    testBall2.setPosition({ 400, 400 });
    testBall2.setFillColor(sf::Color::Green);
    testBall2.setOrigin({ testBall2.getRadius(), testBall2.getRadius() });

    
    // Useful parameters
    float poolCuePower = 2;

    bool paused = false;
    sf::Vector2f mouseForceVector{};
    sf::Clock clock;
    while (window.isOpen()) {

        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (keyPressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                    sf::Vector2f mouseToBallVector = testBall.getPosition() - localMousePosition;

                    float distance = std::sqrt(mouseToBallVector.x * mouseToBallVector.x + mouseToBallVector.y * mouseToBallVector.y);

                    if (distance < testBall.getRadius()) {
                        mouseForceVector = testBall.getPosition();
                        paused = true;
                    }
                }
                    
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (keyReleased->button == sf::Mouse::Button::Left) {
                    sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                    mouseForceVector = mouseForceVector - localMousePosition;
                    testBall.addForce(mouseForceVector * poolCuePower);

                    paused = false;
                }
            }
        }
        if (!paused) {
            // Update physics
            testBall.updatePosition(deltaTime, window.getSize());
            testBall2.updatePosition(deltaTime, window.getSize());
        }
        
        window.clear();
        window.draw(testBall);
        window.draw(testBall2);
        window.display();
    }
}