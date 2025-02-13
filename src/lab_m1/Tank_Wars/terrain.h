#pragma once

#include <vector>
#include "utils/glm_utils.h"

namespace terrain
{
	float func(std::vector<float> A, std::vector<float> Omega, float point);
	std::vector<std::tuple<float, float>> generate_terrain(int width, int height);
}   // namespace terrain