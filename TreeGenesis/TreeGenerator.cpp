#include "TreeGenerator.h"

namespace Tree {

	Generator::Generator(Crown& crown, const float MaxDistance, const float MinDistance, const float branchLength)
		: crown(crown), MaxDistance(MaxDistance), MinDistance(MinDistance), branchLength(branchLength), MaxDistanceSQ(MaxDistance*MaxDistance), MinDistanceSQ(MinDistance*MinDistance)
	{
	}
	Generator::~Generator()
	{
	}

	void Generator::add_trunk(const vec3f& base) {
		
		trunks.push_back(branches.size());
		branches.push_back(Branch(base));

		while (true) {
			unsigned branchesSize = branches.size();
			Branch& LB = branches[branchesSize - 1];

			for (vec3f& point : crown.points) {
				float distSQ = (point.x - LB.position.x)*(point.x - LB.position.x) + (point.y - LB.position.y)*(point.y - LB.position.y) + (point.z - LB.position.z)*(point.z - LB.position.z);
				if (distSQ < MaxDistanceSQ)
					return;
			}

			LB.childrenID.push_back(branchesSize);
			branches.push_back(Branch(vec3f(LB.position.x, LB.position.y + branchLength, LB.position.z), branchesSize - 1));
		}
	}

	unsigned int Generator::process() {
		unsigned int BranchesAdded = 0;

		for (int i = crown.points.size() -1; i--> 0; ) {
			vec3f& point = crown.points[i];

			float closeDistSQ = MaxDistanceSQ * 2;
			Branch* closeBranch = nullptr;

			for (Branch& br : branches) {
				float distSQ = (point.x - br.position.x)*(point.x - br.position.x) + (point.y - br.position.y)*(point.y - br.position.y) + (point.z - br.position.z)*(point.z - br.position.z);

				if (distSQ > MaxDistanceSQ) {
					continue;
				}
				else if (distSQ < MinDistanceSQ) {
					crown.points.erase(crown.points.begin() + i);
					closeBranch = nullptr;
					break;
				}
				else if (distSQ < closeDistSQ) {
					closeDistSQ = distSQ;
					closeBranch = &br;
				}
			}

			if (closeBranch != nullptr) {
				closeBranch->GrowCount++;

				//closeBranch->GrowDirection += point - closeBranch->position;
				closeBranch->GrowDirection += (point - closeBranch->position).normalize();
			}
		}

		for (int i = 0, n = branches.size(); i < n; i++) {
			Branch& br = branches[i];
			if (br.GrowCount > 0) {
				br.childrenID.push_back(branches.size());
				branches.push_back(br.grow(branchLength, i));
				BranchesAdded++;
			}
		}
		
		return BranchesAdded;
	}

	void Generator::processAll(const unsigned& maxIter) {
		if (maxIter == 0) {
			while (process() != 0);
		}
		else {
			for (unsigned i = 0; i < maxIter && process() != 0; i++);
		}
	}
}
