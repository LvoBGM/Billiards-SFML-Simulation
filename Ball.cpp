#include "Ball.h"

void Ball::addForce(sf::Vector2f force) {
	m_forces.emplace_back(force);
}

Ball& Ball::makeBall(float size, sf::Vector2f position, sf::Color color) {
	auto ballPtr = std::make_unique<Ball>(size, size);
	ballPtr->setPosition(position);
	ballPtr->setFillColor(color);
	ballPtr->setOrigin({ ballPtr->getRadius(), ballPtr->getRadius() });

	s_Balls.push_back(std::move(ballPtr));
	return *ballPtr;
}

extern float frictionCoefficient;
extern float minimumFriction;
void Ball::calcFuturePos(const float& dt, const sf::Vector2u& windowSize) {
	// Update members
	m_dt = dt;
	m_windowSize = windowSize;

	// Calculates the final force before collision
	calculateNextPosition(dt, windowSize);

	// Check for wall collisions
	wallCheck(getPosition());

	// Conserve force to next frame
	addForce(m_nextForce);

	setPosition(m_futurePosition);
}

void Ball::calculateNextPosition(const float& dt, const sf::Vector2u& windowSize) {
	// Calculate final force
	m_nextForce = { 0.f, 0.f };
	for (auto force : m_forces) {
		m_nextForce += force;
	}
	m_forces.clear();

	m_nextForce *= std::pow(1.f - (frictionCoefficient), dt);

	// Adding fixed friction
	float speed = m_nextForce.length();
	float lengthAfterMinFriction = speed - minimumFriction;
	if (lengthAfterMinFriction <= 0) {
		m_nextForce = { 0, 0 }; // If ball is reaaaally slow and the length becomes negative, we just stop the ball
	}
	else {
		m_nextForce *= (lengthAfterMinFriction / speed); // Add constant friction, for more realistic stop
	}

	m_futurePosition = getPosition() + m_nextForce * m_dt;
}

// Check if we're outside the window
void Ball::wallCheck(const sf::Vector2f& position) {
	// Check if we are going to hit a wall
	auto radius = getRadius();

	sf::Vector2f offset{};
	if (m_futurePosition.x - radius < 0.f) { // Left side
		float oldLength = m_nextForce.length();
		float distanceToEdge = getPosition().x - radius;
		sf::Vector2f distanceToEdgeVector{ distanceToEdge, 0 };
		sf::Angle angle = distanceToEdgeVector.angleTo(m_nextForce);

		// Secant of 90 degrees is undefined
		if (angle != sf::degrees(90)) {
			offset = sf::Vector2f{ distanceToEdge * (1.f / std::cosf(angle.asRadians())), m_nextForce.angle() };

			sf::Vector2f bounceForce = { oldLength * m_dt - offset.length(), -offset.angle() };
			offset += bounceForce;

			m_futurePosition = getPosition() + offset;

			// Conserve force to the next frame
			m_nextForce = sf::Vector2f{ std::abs(oldLength - bounceForce.length()), bounceForce.angle() };
		}
	}
	if (m_futurePosition.x + radius > m_windowSize.x) { // Right side
		float oldLength = m_nextForce.length();
		float distanceToEdge = m_windowSize.x - getPosition().x - radius;
		sf::Vector2f distanceToEdgeVector{ distanceToEdge, 0 };
		sf::Angle angle = distanceToEdgeVector.angleTo(m_nextForce);

		// Secant of 90 degrees is undefined
		if (angle != sf::degrees(90)) {
			offset = sf::Vector2f{ distanceToEdge * (1.f / std::cosf(angle.asRadians())), m_nextForce.angle() };

			sf::Vector2f bounceForce = { oldLength * m_dt - offset.length(), sf::degrees(180) - offset.angle() };
			offset += bounceForce;

			m_futurePosition = getPosition() + offset;

			// Conserve force to the next frame
			m_nextForce = sf::Vector2f{ std::abs(oldLength - bounceForce.length()), bounceForce.angle() };
		}
	}
	if (m_futurePosition.y - radius < 0.f) { // Top side
		float oldLength = m_nextForce.length();
		float distanceToEdge = getPosition().y - radius;
		sf::Vector2f distanceToEdgeVector{ 0, distanceToEdge };
		sf::Angle angle = distanceToEdgeVector.angleTo(m_nextForce);

		// Secant of 90 degrees is undefined
		if (angle != sf::degrees(90)) {
			offset = sf::Vector2f{ distanceToEdge * (1.f / std::cosf(angle.asRadians())), m_nextForce.angle() };

			sf::Vector2f bounceForce = { oldLength * m_dt - offset.length(), sf::degrees(180) - offset.angle() };
			offset += bounceForce;

			m_futurePosition = getPosition() + offset;

			// Conserve force to the next frame
			m_nextForce = sf::Vector2f{ std::abs(oldLength - bounceForce.length()), bounceForce.angle() };
		}
	}
	if (m_futurePosition.y + radius > m_windowSize.y) { // Bottom side
		float oldLength = m_nextForce.length();
		float distanceToEdge = m_windowSize.y - getPosition().y - radius;
		sf::Vector2f distanceToEdgeVector{ 0, distanceToEdge };
		sf::Angle angle = distanceToEdgeVector.angleTo(m_nextForce);

		// Secant of 90 degrees is undefined
		if (angle != sf::degrees(90)) {
			offset = sf::Vector2f{ distanceToEdge * (1.f / std::cosf(angle.asRadians())), m_nextForce.angle() };

			sf::Vector2f bounceForce = { oldLength * m_dt - offset.length(), -offset.angle() };
			offset += bounceForce;

			m_futurePosition = getPosition() + offset;

			// Conserve force to the next frame
			m_nextForce = sf::Vector2f{ std::abs(oldLength - bounceForce.length()), bounceForce.angle() };
		}
	}
}

//// Takes a force Vector and a hit ball, adds a force to the hit ball and returns the final force
//sf::Vector2f Ball::_ballCollision(Ball* hitBall, const sf::Vector2f& finalForce, bool isHitterBall) {
//	auto thisToBallVector = hitBall->getPosition() - getPosition();
//
//	// Calculate angle of the new force of the hitter ball
//	sf::Angle beta = finalForce.angleTo(thisToBallVector);
//
//	// If beta is more than 90, a collision is not possible
//	if (beta > sf::degrees(90) || beta < sf::degrees(-90)) {
//		return finalForce;
//	}
//	
//	/*std::cout << "{ " << finalForce.normalized().x << ", " << finalForce.normalized().y << " }" << " angle to"
//		<< "{ " << thisToBallVector.normalized().x << ", " << thisToBallVector.normalized().y << " }" << " = " <<
//		beta.asDegrees() << std::endl;*/
//
//	sf::Angle alpha;
//	if (beta < sf::degrees(0)) {
//		alpha = sf::degrees(-90) - beta;
//	}
//	else {
//		alpha = sf::degrees(90) - beta;
//	}
//
//	//std::cout << getFillColor().toInteger() << " Ball hit at angle: " << beta.asDegrees() << std::endl;
//	
//	// Check if angle is close to 180
//	if (std::abs((beta - sf::radians(3.1415f)).asRadians()) < 0.01f) {
//		hitBall->addForce(-finalForce);
//		return -finalForce;
//	}
//
//	float finalForceLength = sqrt(finalForce.x*finalForce.x + finalForce.y*finalForce.y);
//
//	// Hitter ball
//	sf::Angle newFinalForceAngle = finalForce.angle() - alpha;
//	float newFinalForceLength = finalForceLength * std::sin(std::abs(beta.asRadians()));
//
//	// Hit ball
//	sf::Angle hitBallAngle = finalForce.angle() + beta;
//	float hitBallLength = finalForceLength * std::sin(std::abs(alpha.asRadians()));
//
//	//std::cout << "Hitter vector: " << newFinalForceLength << ", " << newFinalForceAngle.asDegrees() << std::endl;
//	sf::Vector2f hitterBallVector = { newFinalForceLength, newFinalForceAngle };
//	sf::Vector2f hitBallVector = { hitBallLength, hitBallAngle };
//
//	if (isHitterBall) {
//		// Both balls are now in balls
//		m_insideBall = true;
//		hitBall->setInsideBall(true);
//
//		if (hitBall->calculateFinalForce(m_dt, m_windowSize) != sf::Vector2f{ 0,0 }) {
//			hitBall->movingCollision(m_dt, m_windowSize, this);
//		}
//	}
//
//	//std::cout << "Pushing hit ball with vector: " << hitBallVector.x << ", " << hitBallVector.y << std::endl;
//	//std::cout << "Pushing hitter ball with vector " << hitterBallVector.x << ", " << hitBallVector.y << std::endl;
//
//	hitBall->addForce(hitBallVector);
//	return hitterBallVector;
//	
//}
