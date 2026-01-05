#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Ball.h"
#include "Arrow.h"

float frictionCoefficient = 0.25f; // 0.25
float minimumFriction = 100.f; //0.1

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1200, 900 }), "Billiards");
    //window.setFramerateLimit(15);

    Ball::makeBall(50, { 200, 200 }, sf::Color::Red);

    // Useful parameters
    float poolCuePower = 2;

    bool paused = false;
    bool rightMousePressed = false;
    bool tPressed = false;
    Ball* selectedBall = nullptr;
    std::unique_ptr<Arrow> arrowPtr = nullptr;
    sf::Clock clock;
    while (window.isOpen()) {

        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                // Grab ball
                if (keyPressed->button == sf::Mouse::Button::Left) {
                    if (!paused) {
                        for (const std::unique_ptr<Ball>& ball : Ball::s_Balls) {
                            sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                            sf::Vector2f mouseToBallVector = ball->getPosition() - localMousePosition;

                            // Ball grabbed
                            if (!paused) {
                                if (mouseToBallVector.length() < ball->getRadius()) {
                                    selectedBall = ball.get();

                                    // Make Arrow
                                    auto mirroredMousePosition = sf::Vector2f(selectedBall->getPosition().x * 2, selectedBall->getPosition().y * 2) - localMousePosition;
                                    arrowPtr = std::make_unique<Arrow>(selectedBall->getPosition(), mirroredMousePosition, selectedBall->getFillColor());

                                    selectedBall = ball.get();
                                    paused = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                // Place ball
                else if (keyPressed->button == sf::Mouse::Button::Right && !paused) {
                    sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                    Ball::makeBall(50, localMousePosition, sf::Color::Green);
                }
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                // Ball launched
                if (keyReleased->button == sf::Mouse::Button::Left) {
                    if (paused) {
                        sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                        selectedBall->setNextForce(arrowPtr->getVector() * poolCuePower);

                        selectedBall = nullptr;
                        arrowPtr = nullptr;

                        paused = false;
                    }
                }
                else if (keyReleased->button == sf::Mouse::Button::Right) {
                    rightMousePressed = false;
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::T) {
                    tPressed = !tPressed;
                }
            }
        }
        // Update physics
        if (!paused) {
            for (const std::unique_ptr<Ball>& ball : Ball::s_Balls) {
                ball->calcFuturePos(deltaTime, window.getSize());
            }
            // Check for collisions
            Ball::checkForCollisions(deltaTime);

            Ball::updatePositions();
        }
        // Ball aimed
        else {
            if (selectedBall) {
                sf::Vector2f localMousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                auto mirroredMousePosition = sf::Vector2f(selectedBall->getPosition().x * 2, selectedBall->getPosition().y * 2) - localMousePosition;

                // Update Arrow
                arrowPtr->setVector(mirroredMousePosition - selectedBall->getPosition());
                arrowPtr->setPosition(selectedBall->getPosition() + arrowPtr->getVector().normalized() * selectedBall->getRadius());
            }
        }

        // Draw screen
        window.clear();
        for (const std::unique_ptr<Ball>& ball : Ball::s_Balls) {
            window.draw(*ball);
        }
        if (tPressed) {
            for (const Arrow* arrow : Arrow::s_Arrows) {
                if (arrow) {
                    window.draw(*arrow);
                }
            }
        }
        
        window.display();
    }
}