#include "Arrow.h"
Arrow::Arrow()
{
	m_vector = {0.f, 0.f};
	m_color = sf::Color::Red;

	setPosition({ 0, 0 });
}
Arrow::Arrow(sf::Vector2f origin, sf::Vector2f end, sf::Color color) {
	// Add arrow to s_Arrows
	s_ArrowsToDraw.insert(this);

	m_vector = end - origin;
	m_color = color;
	this->setPosition(origin);

	UpdateGeometry();
}

Arrow::~Arrow() {
	s_ArrowsToDraw.erase(this);
}

// Adds (or removes) the arrow from the ArrowsToDraw set
void Arrow::toBeDrawn(bool b)
{
	if (b) {
		s_ArrowsToDraw.insert(this);
	}
	else {
		s_ArrowsToDraw.erase(this);
	}
}

void Arrow::setVector(const sf::Vector2f& v) {
	m_vector = v;
	UpdateGeometry();
}

void Arrow::setColor(const sf::Color & color) {
	for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
	{
		m_vertices[i].color = sf::Color::Red;
	}
}

sf::Vector2f Arrow::getVector() {
	return m_vector;
}

void Arrow::UpdateGeometry() {
	// Arrow smallness
	constexpr float lengthScale = 3;
	float size = m_vector.length() / lengthScale;

	float height = size;
	float width = size * m_widthHeightRatio;

	// Place vertexes (Arrow points to the right)
	m_vertices = sf::VertexArray(sf::PrimitiveType::TriangleStrip, 7);

	m_vertices[0] = { -sf::Vector2f(0.f, width / 2), m_color };
	m_vertices[1] = { sf::Vector2f(0.f, width / 2), m_color };
	m_vertices[2] = { m_vertices[0].position + sf::Vector2f(height * (1 - m_headTailRatio), 0.f), m_color };
	m_vertices[3] = { m_vertices[1].position + sf::Vector2f(height * (1 - m_headTailRatio), 0.f), m_color };
	m_vertices[4] = { m_vertices[3].position + sf::Vector2f(0.f, width), m_color };
	m_vertices[5] = { sf::Vector2f(height, 0.f), m_color };
	m_vertices[6] = { m_vertices[2].position - sf::Vector2f(0.f, width), m_color };
;
	this->setRotation(m_vector.angle());

}
