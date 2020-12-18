#include "TreeBranch.h"


namespace Tree {

	Branch::Branch(vec3f position)
		: GrowDirection{0,0,0}, GrowCount(0), position(position)
	{
	}
	Branch::Branch(vec3f position, unsigned int parentID)
		: GrowDirection{0,0,0}, GrowCount(0), position(position), parentID(parentID)
	{
	}
	Branch::~Branch()
	{
	}

	Branch Branch::grow(const float& branchLength, const unsigned int& parentID) {
		//Tree::vec3f newposition = (GrowDirection / GrowCount * branchLength) + position + (Tree::random_vec3f() * (branchLength * 0.1f));
		Tree::vec3f newposition = (GrowDirection.normalize() * branchLength) + position + (Tree::vec3f::random() * (branchLength * 0.1f));
		GrowCount = 0;
		GrowDirection.x = GrowDirection.y = GrowDirection.z = 0;

		return Branch(newposition, parentID);
	}
}