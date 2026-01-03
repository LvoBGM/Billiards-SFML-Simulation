#include "Ball.h"

sf::Vector2f Ball::getNextForce()
{
	return m_nextForce;
}

sf::Vector2f Ball::getFuturePosition()
{
	return m_futurePosition;
}

sf::Vector2f Ball::getForceArrowVector()
{
	return m_forceArrow->getVector();
}

void Ball::setFuturePosition(const sf::Vector2f& v)
{
	m_futurePosition = v;
}

void Ball::setNextForce(const sf::Vector2f v)
{
	m_nextForce = v;
}

void Ball::setForceArrow(sf::Vector2f origin, sf::Vector2f end, sf::Color color) {
	m_forceArrow = std::make_unique<Arrow>(origin, end, color);
}

void Ball::setForceArrowVector(const sf::Vector2f& v)
{
	m_forceArrow->setVector(v);
}

void Ball::setForceArrowPosition(const sf::Vector2f& v)
{
	m_forceArrow->setPosition(v);
}

Ball& Ball::makeBall(float size, sf::Vector2f position, sf::Color color) {
	auto ballPtr = std::make_unique<Ball>(size, size);
	ballPtr->setPosition(position);
	ballPtr->setFillColor(color);
	ballPtr->setOrigin({ ballPtr->getRadius(), ballPtr->getRadius() });
	ballPtr->setForceArrow(position, position, color);

	s_Balls.push_back(std::move(ballPtr));
	return *s_Balls.back();
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
	auto position = getPosition();
	while (_wallCheck(position)) {
		position = m_futurePosition;
		std::cout << "Hit wall!\n";
	}
}

void Ball::calculateNextPosition(const float& dt, const sf::Vector2u& windowSize) {

	m_nextForce *= std::pow(1.f - (frictionCoefficient), dt);

	// Adding fixed friction
	float speed = m_nextForce.length();
	float lengthAfterMinFriction = speed - minimumFriction * m_dt;
	if (lengthAfterMinFriction <= 0) {
		m_nextForce = { 0, 0 }; // If ball is reaaaally slow and the length becomes negative, we just stop the ball
	}
	else {
		m_nextForce *= (lengthAfterMinFriction / speed); // Add constant friction, for more realistic stop
	}

	m_futurePosition = getPosition() + m_nextForce * m_dt;
}

// Check if we're outside the window
bool Ball::_wallCheck(const sf::Vector2f& position) {
	// Check if we are going to hit a wall
	auto radius = getRadius();
	bool hitWall = false;

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

			hitWall = true;
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

			hitWall = true;
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

			hitWall = true;
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

			hitWall = true;
		}
	}
	return hitWall;
}

// Check for any collisions between balls and update their vectors accordingly
void Ball::checkForCollisions(const float& dt)
{
	for (std::size_t i = 0; i < s_Balls.size(); i++) {
		for (std::size_t j = i + 1; j < s_Balls.size(); j++) {
			// Set up balls
			const std::unique_ptr<Ball>& ball1 = s_Balls[i];
			const std::unique_ptr<Ball>& ball2 = s_Balls[j];

			auto vectorBetweenBalls = ball1->getPosition() - ball2->getPosition();

			// Distance between the balls
			float distance = vectorBetweenBalls.length() - ball1->getRadius() - ball2->getRadius();

			if (distance < 0) {

				std::pair<sf::Vector2f, sf::Vector2f> iHitJ1 = ballCollision(ball1, ball2);
				std::pair<sf::Vector2f, sf::Vector2f> jHitI1 = ballCollision(ball2, ball1);

				if (iHitJ1 == std::pair<sf::Vector2f, sf::Vector2f>{ {0, 0}, { 0, 0 } }&& iHitJ1 == std::pair<sf::Vector2f, sf::Vector2f> { {0, 0}, { 0, 0 } }) {
					std::cout << "dsada";
				}

				// Get pairs of new velocities
				std::pair<sf::Vector2f, sf::Vector2f> iHitJ = ballCollision(ball1, ball2);
				std::pair<sf::Vector2f, sf::Vector2f> jHitI = ballCollision(ball2, ball1);

				// If both collisions return 0, then no collision took place
				if (iHitJ == std::pair<sf::Vector2f, sf::Vector2f>{ {0, 0}, { 0, 0 } }&& jHitI == std::pair<sf::Vector2f, sf::Vector2f> { {0, 0}, { 0, 0 } }) {
					return;
				}

				// Set new nextForce
				ball1->setNextForce((iHitJ.first + jHitI.second));
				ball2->setNextForce((iHitJ.second + jHitI.first));

				// Recalculate future position
				ball1->setFuturePosition(ball1->getPosition() + ball1->getNextForce() * dt);
				ball2->setFuturePosition(ball2->getPosition() + ball2->getNextForce() * dt);

				if (ball1->getNextForce().length() != 0) {
					std::cout << ball1->getNextForce().length() << std::endl;
				}
			}
		}
	}
}

// Takes two unique pointers to balls, one a hitter and the other a stationary hit Ball, and returns a pair of the new force vectors of the balls
std::pair<sf::Vector2f, sf::Vector2f> Ball::ballCollision(const std::unique_ptr<Ball>& hitterBall, const std::unique_ptr<Ball>& hitBall) {
	// Check if hitter ball is not moving
	if (hitterBall->getNextForce() == sf::Vector2f{ 0,0 }) {
		return { {0, 0}, {0, 0} };
	}
	
	auto thisToBallVector = hitBall->getPosition() - hitterBall->getPosition();
	auto hitterNextForce = hitterBall->getNextForce();

	// Calculate angle between
	sf::Angle beta = hitterNextForce.angleTo(thisToBallVector);

	// If angle is more than 90 degrees a collision is not possible
	if (beta > sf::degrees(90) || beta < sf::degrees(-90)) {
		return { {0, 0}, {0, 0} };
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
		return { {0, 0}, hitterNextForce };
	}

	float nextForceLength = hitterNextForce.length();

	// Hitter ball
	sf::Angle newFinalForceAngle = hitterNextForce.angle() - alpha;
	float newFinalForceLength = nextForceLength * std::sin(std::abs(beta.asRadians()));

	// Hit ball
	sf::Angle hitBallAngle = hitterNextForce.angle() + beta;
	float hitBallLength = nextForceLength * std::sin(std::abs(alpha.asRadians()));

	//std::cout << "Hitter vector: " << newFinalForceLength << ", " << newFinalForceAngle.asDegrees() << std::endl;
	sf::Vector2f hitterBallVector = { newFinalForceLength, newFinalForceAngle };
	sf::Vector2f hitBallVector = { hitBallLength, hitBallAngle };

	//std::cout << "Pushing hit ball with vector: " << hitBallVector.x << ", " << hitBallVector.y << std::endl;
	//std::cout << "Pushing hitter ball with vector " << hitterBallVector.x << ", " << hitBallVector.y << std::endl;

	return { hitterBallVector, hitBallVector };
	
}

void Ball::updatePositions()
{
	for (const std::unique_ptr<Ball>& ball : s_Balls) {
		ball->setPosition(ball->getFuturePosition());
		ball->setForceArrowVector(ball->getNextForce());
		ball->setForceArrowPosition(ball->getPosition() + ball->getForceArrowVector().normalized() * ball->getRadius());
	}
}
