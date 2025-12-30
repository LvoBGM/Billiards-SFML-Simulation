#include "Arrow.h"
void Arrow::setColor(const sf::Color & color) {
	for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
	{
		m_vertices[i].color = sf::Color::Red;
	}
}
