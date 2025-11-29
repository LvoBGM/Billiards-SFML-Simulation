#include "Ball.h"

void Ball::addForce(sf::Vector2f force) {
	m_forces.emplace_back(force);
}

void Ball::setInsideBall(bool b) {
	m_insideBall = b;
}

extern float frictionCoefficient;
extern float minimumFriction;
void Ball::updatePosition(const float& dt, const sf::Vector2u& windowSize) {

	// Calculate final force
	sf::Vector2f finalForce{ 0.f, 0.f };
	for (auto force : m_forces) {
		finalForce += force;
	}
	m_forces.clear();

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

	bool hitBall = false;
	// Check if we've hit a ball
	for (Ball* ball : s_Balls) {
		auto thisToballVector = ball->getPosition() - position;

		float distance = std::sqrt(thisToballVector.x * thisToballVector.x + thisToballVector.y * thisToballVector.y);

		if (distance - getRadius() < ball->getRadius()) {
			if (ball == this) {
				continue;
			}
			else {
				hitBall = true;
				if (!m_insideBall && finalForce != sf::Vector2f{0,0}) {
					_ballCollision(ball, finalForce);
				}
				else {
					//std::cout << "InsideBall!" << std::endl;
				}
			}
		}
	}

	if (!hitBall) {
		m_insideBall = false;
	}

	// We conserve the momentum from the previous position update
	m_forces.emplace_back(finalForce);

	move({ finalForce.x * dt, finalForce.y * dt });
}

// Takes a force Vector and a hit ball, adds a force to the hit ball and changes the final force
void Ball::_ballCollision(Ball* hitBall, sf::Vector2f& finalForce) {
	auto thisToBallVector = hitBall->getPosition() - getPosition();

	// Calculate angle of the new force of the hitter ball
	sf::Angle beta = finalForce.angleTo(thisToBallVector);

	// If beta is more than 90, a collision is not possible
	if (beta > sf::degrees(90) && beta < sf::degrees(-90)) {
		return;
	}
	
	/*std::cout << "{ " << finalForce.normalized().x << ", " << finalForce.normalized().y << " }" << " angle to"
		<< "{ " << thisToBallVector.normalized().x << ", " << thisToBallVector.normalized().y << " }" << " = " <<
		beta.asDegrees() << std::endl;*/

	sf::Angle alpha;
	if (beta < sf::degrees(0)) {
		alpha = sf::degrees(-90) - beta;
	}
	else {
		alpha = sf::degrees(90) - beta;
	}

	//std::cout << getFillColor().toInteger() << " Ball hit at angle: " << beta.asDegrees() << std::endl;
	
	// Check if angle is close to 180
	if (std::abs((beta - sf::radians(3.1415f)).asRadians()) < 0.01f) {
		finalForce = -finalForce;
		hitBall->addForce(finalForce);
		return;
	}

	float finalForceLength = sqrt(finalForce.x*finalForce.x + finalForce.y*finalForce.y);

	// Hitter ball
	sf::Angle newFinalForceAngle = finalForce.angle() - alpha;
	float newFinalForceLength = finalForceLength * std::sin(std::abs(beta.asRadians()));

	// Hit ball
	sf::Angle hitBallAngle = finalForce.angle() + beta;
	float hitBallLength = finalForceLength * std::sin(std::abs(alpha.asRadians()));

	//std::cout << "Hitter vector: " << newFinalForceLength << ", " << newFinalForceAngle.asDegrees() << std::endl;
	sf::Vector2f hitterBallVector = { newFinalForceLength, newFinalForceAngle };
	sf::Vector2f hitBallVector = { hitBallLength, hitBallAngle };

	finalForce = hitterBallVector;
	hitBall->addForce(hitBallVector);

	// Both balls are now in balls
	m_insideBall = true;
	hitBall->setInsideBall(true);

	//std::cout << "Pushing hit ball with vector: " << hitBallVector.x << ", " << hitBallVector.y << std::endl;
	//std::cout << "Pushing hitter ball with vector " << hitterBallVector.x << ", " << hitBallVector.y << std::endl;
}
