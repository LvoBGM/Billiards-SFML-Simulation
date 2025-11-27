#include "Ball.h"

void Ball::AddForce(sf::Vector2f force) {
	m_forces.emplace_back(force);
}

extern float frictionCoefficient;
extern float minimumFriction;
void Ball::UpdatePosition(const float& dt, const sf::Vector2u& windowSize) {

	// Calculate final force
	sf::Vector2f finalForce{ 0.f, 0.f };
	for (auto force : m_forces) {
		finalForce += force;
	}
	m_forces.clear();

	// Check if we've hit a wall
	auto position = getPosition();
	auto radius = getRadius();
	if (position.x - radius <= 0.f) { // Left side
		finalForce = { -finalForce.x, finalForce.y };
	}
	if (position.x + radius >= windowSize.x) { // Right side
		finalForce = { -finalForce.x, finalForce.y };
	}
	if (position.y - radius <= 0.f) { // Top side
		finalForce = { finalForce.x, -finalForce.y };
	}
	if (position.y + radius >= windowSize.y) { // Bottom side
		finalForce = { finalForce.x, -finalForce.y };
	}

	// Multiply by delta t and add friction
	float speed = std::sqrt(finalForce.x * finalForce.x + finalForce.y * finalForce.y);

	finalForce *= std::pow(1.f - (frictionCoefficient), dt);

	float lengthAfterMinFriction = speed - minimumFriction;
	if (lengthAfterMinFriction <= 0) {
		finalForce = {}; // If ball is reaaaally slow and the length becomes negative, we just stop the ball
	}
	else {
		finalForce *= (lengthAfterMinFriction / speed); // Add constant friction, for more realistic stop
	}

	// We conserve the momentum from the previous position update
	m_forces.emplace_back(finalForce);

	move({ finalForce.x * dt, finalForce.y * dt });
}
