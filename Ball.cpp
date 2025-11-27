#include "Ball.h"

void Ball::AddForce(sf::Vector2f force) {
	m_forces.emplace_back(force);
}
void Ball::UpdatePosition(const float& dt, const sf::Vector2u& windowSize) {

	// Calculate final force
	sf::Vector2f finalForce{0.f, 0.f};
	for (auto force : m_forces) {
		finalForce += force;
	}
	m_forces.clear();

	// Check if we've hit a wall
	auto position = getPosition();
	auto radius = getRadius();
	if (position.x - radius <= 0.f) { // Left side

	}
	if (position.x + radius >= windowSize.x) { // Right side

	}
	if (position.y - radius <= 0.f) { // Top side

	}
	if (position.y + radius >= windowSize.y) { // Bottom side

	}

	// We conserve the momentum from the previous position update
	m_forces.emplace_back(finalForce);

	// Finally, we update the position
	finalForce = { finalForce.x * dt, finalForce.y * dt };
	move(finalForce);
}
