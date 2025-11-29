#include "Ball.h"

void Ball::addForce(sf::Vector2f force) {
	m_forces.emplace_back(force);
}

void Ball::setInsideBall(bool b) {
	m_insideBall = b;
}

Ball& Ball::makeBall(float size, sf::Vector2f position, sf::Color color) {
	auto ballPtr = std::make_unique<Ball>(size, size);
	ballPtr->setPosition(position);
	ballPtr->setFillColor(color);
	ballPtr->setOrigin({ ballPtr->getRadius(), ballPtr->getRadius() });

	s_Balls.push_back(std::move(ballPtr));
	return *ballPtr;
}

void Ball::movingCollision(const float& dt, const sf::Vector2u& windowSize, Ball* hitterBall) {
	sf::Vector2f finalForce = m_calculatedFinalForce;

	_ballCollision(hitterBall, finalForce, false);
}

extern float frictionCoefficient;
extern float minimumFriction;
void Ball::updatePosition(const float& dt, const sf::Vector2u& windowSize) {
	// Update members
	m_dt = dt;
	m_windowSize = windowSize;

	sf::Vector2f finalForce;
	if (!m_finalForceCalculated) {
		finalForce = calculateFinalForce(dt, windowSize);
	}
	else {
		finalForce = m_calculatedFinalForce;
	}
	

	_outOfBoundCheck(getPosition(), getRadius());

	// Check if we've hit a ball
	bool hitBall = false;
	for (const std::unique_ptr<Ball>& ball : s_Balls) {
		auto thisToballVector = ball->getPosition() - getPosition();

		float distance = std::sqrt(thisToballVector.x * thisToballVector.x + thisToballVector.y * thisToballVector.y);

		if (distance - getRadius() < ball->getRadius()) {
			if (ball.get() == this) {
				continue;
			}
			else {
				hitBall = true;
				if (!m_insideBall && finalForce != sf::Vector2f{0,0}) {
					finalForce = _ballCollision(ball.get(), finalForce);
				}
			}
		}
	}

	if (!hitBall) {
		m_insideBall = false;
	}

	// We conserve the momentum from the previous position update
	m_forces.emplace_back(finalForce);
	m_finalForceCalculated = false;

	move({ finalForce.x * dt, finalForce.y * dt });
}

sf::Vector2f Ball::calculateFinalForce(const float& dt, const sf::Vector2u& windowSize) {
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
	float wallPushBackForce = 5 * dt;
	if (position.x - radius <= 0.f) { // Left side
		finalForce = { -finalForce.x, finalForce.y };
		finalForce += { wallPushBackForce, 0};
	}
	if (position.x + radius >= windowSize.x) { // Right side
		finalForce = { -finalForce.x, finalForce.y };
		finalForce += { -wallPushBackForce, 0 };
	}
	if (position.y - radius <= 0.f) { // Top side
		finalForce = { finalForce.x, -finalForce.y };
		finalForce += { 0, wallPushBackForce };
	}
	if (position.y + radius >= windowSize.y) { // Bottom side
		finalForce = { finalForce.x, -finalForce.y };
		finalForce += { 0, -wallPushBackForce };
	}

	m_finalForceCalculated = true;
	m_calculatedFinalForce = finalForce;
	return finalForce;
}

// Check if we're outside the window
void Ball::_outOfBoundCheck(const sf::Vector2f& position, const float& radius) {
	
	if (position.x < 0) {
		setPosition({ radius, position.y });
	}
	else if (position.x > m_windowSize.x) {
		setPosition({ m_windowSize.x - radius, position.y });
	}
	if (position.y < 0) {
		setPosition({ position.x, radius });
	}
	else if (position.y > m_windowSize.y) {
		setPosition({ position.x, m_windowSize.y - radius });
	}
}

// Takes a force Vector and a hit ball, adds a force to the hit ball and returns the final force
sf::Vector2f Ball::_ballCollision(Ball* hitBall, const sf::Vector2f& finalForce, bool isHitterBall) {
	auto thisToBallVector = hitBall->getPosition() - getPosition();

	// Calculate angle of the new force of the hitter ball
	sf::Angle beta = finalForce.angleTo(thisToBallVector);

	// If beta is more than 90, a collision is not possible
	if (beta > sf::degrees(90) || beta < sf::degrees(-90)) {
		return finalForce;
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
		hitBall->addForce(-finalForce);
		return -finalForce;
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

	if (isHitterBall) {
		// Both balls are now in balls
		m_insideBall = true;
		hitBall->setInsideBall(true);

		if (hitBall->calculateFinalForce(m_dt, m_windowSize) != sf::Vector2f{ 0,0 }) {
			hitBall->movingCollision(m_dt, m_windowSize, this);
		}
	}

	//std::cout << "Pushing hit ball with vector: " << hitBallVector.x << ", " << hitBallVector.y << std::endl;
	//std::cout << "Pushing hitter ball with vector " << hitterBallVector.x << ", " << hitBallVector.y << std::endl;

	hitBall->addForce(hitBallVector);
	return hitterBallVector;
	
}
