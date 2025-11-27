#include <iostream>
#include <SFML/Graphics.hpp>
#include "Ball.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1000, 1000 }), "Billiards");
    
    Ball testBall{ 50, 50 };
    testBall.setPosition({200, 200});
    testBall.setFillColor(sf::Color::Red);
    testBall.AddForce({100.f,100.f});
    
    sf::Clock clock;
    while (window.isOpen()) {

        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (keyPressed->button == sf::Mouse::Button::Left) {
                    
                }
                    
            }
        }
        //sf::Vector2i localPosition = sf::Mouse::getPosition(window);

        // Update physics
        testBall.UpdatePosition(deltaTime, window.getSize());

        window.clear();
        window.draw(testBall);
        window.display();
    }
}