#pragma once

#include <vector>

#include "TreeNode.h"
#include "TreeGenerator.h"


namespace Tree {
	class Nodes {
	public:
		std::vector<tNode> nodes;
		const Generator& generator;

		Nodes(const Tree::Generator& gen);

		// calculate node radiuses
		void initR(const float& r0, const float& n);
		// WARNING: needs to be after findSuccessors() if second ring in non-main-successors parents is desired!
		// calculate num vertex per node + calculate location of vertex data per node
		void initVNandVL(const unsigned& minVn, const unsigned& maxVn);
		// calculate ring rotation directions
		void initDirection();

		void initPFR();
		// init position number from root node

		// reduce number of nodes
		void decimate();
		// smooth branching angles. preferably called after decimation
		void smooth();
		// WARNING: requires R already calculated!
		// find branches that are the biggest children of their parents 
		void findSuccessors();
		// normalize nodes in bounding box
		vec3f normalize(const vec3f& limit);
		vec3f normalize(vec3f limit, const vec3f& origin);
	private:
		float _initR_recursive(tNode& node, const float& r0, const float& n);

		void _initPFR_recursive(tNode & node, unsigned int p);

		bool _decimate_recursive(unsigned currentID = 0, bool canDelete = false);
		bool _deleteNodeAt(const unsigned& currentID = 0);

		void _findSuccessors_recursive(const unsigned& ID, const bool& isMainSuccessor);
	};
}