#pragma once

#include "TreeVector.h"
#include <math.h>
#include <vector>

namespace Tree {
	class Branch
	{
	public:
		vec3f GrowDirection; //a vector
		unsigned int GrowCount;
		vec3f position; //end's position ... the opposite end to the one of the parent
		unsigned parentID = INFINITY;
		std::vector<unsigned> childrenID;
	public:
		Branch(vec3f position);
		Branch(vec3f position, unsigned int parentID);
		~Branch();

		Branch grow(const float& branchLength, const unsigned int& parentID);
	};
}
