#pragma once

#include "TreeCrown.h"
#include "TreeBranch.h"
#include "TreeVector.h"
#include <vector>
#include <math.h>

namespace Tree {

	class Generator
	{
	public:
		Crown& crown;

	private:
		float MaxDistance;
		float MinDistance;
		float branchLength;

		float MaxDistanceSQ;
		float MinDistanceSQ;

	public:
		std::vector<Branch> branches;
		std::vector<unsigned> trunks;

	public:
		Generator(Crown& crown, const float MaxDistance, const float MinDistance, const float branchLength);
		~Generator();

		unsigned int process();
		void processAll(const unsigned& maxIter = 0); // 0 = infinity

		void inline SetMaxDistance(float v) { MaxDistance = v; MaxDistanceSQ = v * v; }
		void inline SetMinDistance(float v) { MinDistance = v; MinDistanceSQ = v * v; }
		void inline SetBranchLength(float v) { branchLength = v; }

		void add_trunk(const vec3f& base); //base is the start point
	};

}
