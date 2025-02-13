#include "T1object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include <iostream>


Mesh* T1object2D::CreateSquare(
	const std::string& name,
	glm::vec3 upRightCorner,
	float length,
	glm::vec3 color,
	bool fill)
{
	glm::vec3 corner = upRightCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, -length, 0), color),
		VertexFormat(corner + glm::vec3(0, -length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 2, 1, 0, 3, 2 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// Draw 2 triangles. Add the remaining 2 indices
		/*indices.push_back(0);
		indices.push_back(2);*/
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* T1object2D::CreateCircle(
	const std::string& name,
	glm::vec3 center,
	float radius,
	glm::vec3 color)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	int numTriangles = 10;
	float angle = 2 * M_PI / numTriangles;

	for (int i = 0; i < numTriangles; ++i) {
		float x = center.x + radius * cos(i * angle);
		float y = center.y + radius * sin(i * angle);
		vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
	}

	for (int i = 0; i < numTriangles - 2; ++i) {
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}

	Mesh* circle = new Mesh(name);
	circle->InitFromData(vertices, indices);
	return circle;
}

Mesh* T1object2D::CreateBarrel(
	const std::string& name,
	glm::vec3 middlePoint,
	float length,
	float height,
	glm::vec3 color)
{
	glm::vec3 corner = middlePoint;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(0, -height / 2, 0), color),
		VertexFormat(corner + glm::vec3(length, -height / 2, 0), color),
		VertexFormat(corner + glm::vec3(length, height / 2, 0), color),
		VertexFormat(corner + glm::vec3(0, height / 2, 0), color)
	};

	Mesh* barrel = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3, 0, 3, 4 };

	barrel->InitFromData(vertices, indices);
	return barrel;
}

Mesh* T1object2D::CreateTankBody(
	const std::string& name,
	glm::vec3 topRightCorner,
	float lengthLower,
	float lengthUpper,
	float additionalLength,
	float height,
	glm::vec3 colorBody,
	glm::vec3 colorLink)
{
	glm::vec3 corner = topRightCorner;

	std::vector<VertexFormat> vertices =
	{
		//Trail
		VertexFormat(corner, colorLink), // A {0}
		VertexFormat(corner + glm::vec3(lengthLower, 0, 0), colorLink), // B {1}
		VertexFormat(corner + glm::vec3(lengthUpper, height, 0), colorLink), // D1 {2}
		VertexFormat(corner + glm::vec3(-lengthUpper / 4, height, 0), colorLink), // C1 {3}

		//Body
		VertexFormat(corner + glm::vec3(lengthUpper, height, 0), colorBody), // D2 {4}
		VertexFormat(corner + glm::vec3(lengthUpper + additionalLength, height, 0), colorBody), // H {5}
		VertexFormat(corner + glm::vec3(lengthLower + additionalLength / 2, height * 2, 0), colorBody), // F {6}
		VertexFormat(corner + glm::vec3(0 - additionalLength / 2, height * 2, 0), colorBody), // E {7}
		VertexFormat(corner + glm::vec3(-lengthUpper / 4 - additionalLength, height, 0), colorBody), // G {8}
		VertexFormat(corner + glm::vec3(-lengthUpper / 4, height, 0), colorBody), // C2 {9}



	};


	Mesh* trapeze = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 3, 3, 1, 2, 7, 8, 9, 7, 9, 4, 7, 4, 6, 6, 4, 5 };


	trapeze->InitFromData(vertices, indices);
	return trapeze;
}