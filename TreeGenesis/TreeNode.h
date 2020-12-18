#pragma once

#include "TreeVector.h"
#include <vector>

namespace Tree {
	struct tNode {
		Tree::vec3f position;
		unsigned parentID;
		std::vector<unsigned> childrenID;
		float R; // radius
		unsigned nV; // num vertex
		unsigned lV; // vertex location in array
		Tree::vec3f direction; // direction
		bool isMainSuccessor; // biggest branch child or root
		unsigned int pos_from_root; // number counting from root node 
	};
}