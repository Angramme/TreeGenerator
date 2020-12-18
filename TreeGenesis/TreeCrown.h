#pragma once

#include "stdlib.h"
#include <vector>
#include <math.h>
#include "TreeVector.h"

namespace Tree {
	class Crown
	{
	public:
		std::vector<vec3f> points;
	public:
		Crown();
		Crown(const unsigned int& numPoints);
		~Crown();
	};

	Crown CircularCrown(const unsigned int& numPoints, const vec3f& center, const unsigned int& r);
	Crown OvalCrown(const unsigned int& numPoints, const vec3f& center, const unsigned int& r1, const unsigned int& r2, const float& rotation);
	Crown BoxCrown(const unsigned int& numPoints, const vec3f& center, const vec3f& size);
}
