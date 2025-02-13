#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace T1object2D
{

	// Create square with given bottom left corner, length and color
	Mesh* CreateSquare(const std::string& name, glm::vec3 upRightCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);
	Mesh* CreateBarrel(const std::string& name, glm::vec3 middlePoint, float length, float height, glm::vec3 color);
	Mesh* CreateTankBody(const std::string& name, glm::vec3 bottomRightCorner, float lengthLower, float lengthUpper, float additionalLength, float height, glm::vec3 colorBody, glm::vec3 colorLink);
}
