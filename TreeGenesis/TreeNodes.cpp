#include "TreeNodes.h"


static inline float remapf(float value, float istart, float istop, float ostart, float ostop) {
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

namespace Tree {

	Nodes::Nodes(const Tree::Generator& gen) 
		: generator(gen)
	{
		for (unsigned i = 0, n = gen.branches.size(); i < n; i++) {
			const Tree::Branch& br = gen.branches[i];
			nodes.push_back({
				br.position,
				br.parentID,
				br.childrenID,
				0,
				0,
				0,
				Tree::vec3f(0,0,0),
				false
				});
		}
	}


	// calculate num vertex per node + calculate location in array of vertex data per node
	void Nodes::initVNandVL(const unsigned& minVn, const unsigned& maxVn) {
		float rMax = 0;
		for (const unsigned& trunkID : generator.trunks) {
			if (nodes[trunkID].R > rMax) {
				rMax = nodes[trunkID].R;
			}
		}

		float r0;
		for (auto iter = nodes.end(); iter != nodes.begin(); iter--) {
			if (iter->childrenID.size() == 0) {
				r0 = iter->R;
				break;
			}
		}

		unsigned offset = 0;
		for (tNode& node : nodes) {
			//node.nV = (node.R - r0) * (float)(maxVn - minVn) / (rMax - r0) + (float)minVn;
			node.nV = remapf(node.R, r0, rMax, minVn, maxVn);
			node.lV = offset;
			//offset += node.nV;
			offset += node.isMainSuccessor ? node.nV : node.nV * 2;
		}
	}


	void Nodes::initDirection() {
		for (unsigned i = 0, n = nodes.size(); i < n; i++) {
			tNode& node = nodes[i];
			tNode& parent = nodes[node.parentID];

			node.direction = node.position - parent.position;
			node.direction.normalize();

			float nodeChildrenSize = node.childrenID.size();
			for (const unsigned& chID : node.childrenID) {
				const tNode& child = nodes[chID];
				node.direction += (child.position - node.position).normalize() / nodeChildrenSize;
			}

			node.direction.normalize();
		}
	}


	void Nodes::smooth() {
		std::vector<Tree::vec3f> oldPos;
		oldPos.reserve(nodes.size());
		for (auto& node : nodes) {
			oldPos.push_back(node.position);
		}

		for (unsigned i = 1, n = nodes.size(); i < n; i++) {
			tNode& node = nodes[i];

			node.position = oldPos[i] + oldPos[node.parentID] / 2;
		}
	}

	void Nodes::decimate() {
		for (const unsigned& ID : generator.trunks) {
			_decimate_recursive(ID);
		}
	}
	bool Nodes::_decimate_recursive(unsigned currentID, bool canDelete) {
		tNode& current = nodes[currentID];
		bool willDelete = canDelete && current.childrenID.size() == 1;

		for (unsigned i = 0, n = current.childrenID.size(); i < n; i++) {
			if (_decimate_recursive(current.childrenID[i], !willDelete)) {
				i--;
				n--;
			}
		}

		if (willDelete) _deleteNodeAt(currentID);

		return willDelete;
	}
	bool Nodes::_deleteNodeAt(const unsigned& currentID) {
		const tNode& current = nodes[currentID];

		if (currentID != 0 && current.childrenID.size() == 1) {
			// copy essential IDs
			unsigned cpID = current.parentID;
			unsigned ccID = current.childrenID[0];

			// delete node
			nodes.erase(nodes.begin() + currentID);

			//	offset node IDs
			for (unsigned i = 0, n = nodes.size(); i < n; i++) {
				tNode& node = nodes[i];

				// offset parentID
				if (node.parentID == currentID) {
					node.parentID = cpID;
				}
				else {
					node.parentID = node.parentID > currentID ? node.parentID - 1 : node.parentID;
				}

				// offset/delete children IDs
				for (auto iter = node.childrenID.begin(); iter != node.childrenID.end(); ) {
					if (*iter == currentID) { // delete ID because the node was deleted
						iter = node.childrenID.erase(iter);
						node.childrenID.push_back(ccID);
					}
					else {
						*iter = *iter > currentID ? *iter - 1 : *iter;
						iter++;
					}
				}
			}
			return true;
		}
		else {
			return false;
		}
	}


	void Nodes::initR(const float& r0, const float& n) {
		for (const unsigned& ID : generator.trunks) {
			_initR_recursive(nodes[ID], r0, n);
		}
	}
	float Nodes::_initR_recursive(tNode& node, const float& r0, const float& n) {
		if (node.childrenID.size() == 0) {
			node.R = r0;
		}
		else {
			float R = 0;
			for (const unsigned& chID : node.childrenID) {
				tNode& child = nodes[chID];

				R += std::powf(
					_initR_recursive(child, r0, n),
					n);
			}
			R = std::powf(R, 1 / n);
			node.R = R;
		}
		return node.R;
	}

	void Nodes::initPFR() {
		for (const unsigned& ID : generator.trunks) {
			_initPFR_recursive(nodes[ID], 0);
		}
	}
	void Nodes::_initPFR_recursive(tNode& node, unsigned int p) {
		if (node.childrenID.size() != 0) {
			for (const unsigned& chID : node.childrenID) {
				tNode& child = nodes[chID];
				_initPFR_recursive(child, p +1);
			}
		}
		node.pos_from_root = p;
	}

	void Nodes::findSuccessors() {
		for (const unsigned& ID : generator.trunks) {
			_findSuccessors_recursive(ID, true); // true because trunks are not connected to anything thus not need to have a second ring in parent.
		}
	}
	void Nodes::_findSuccessors_recursive(const unsigned& ID, const bool& isMainSuccessor) {
		tNode& current = nodes[ID];
		current.isMainSuccessor = isMainSuccessor;

		float bigR = 0;
		unsigned mainSuccessor = ID;

		for (const unsigned& chID : current.childrenID) {
			const tNode& child = nodes[chID];
			if (child.R > bigR) {
				mainSuccessor = chID;
				bigR = child.R;
			}
		}

		for (const unsigned& chID : current.childrenID) {
			_findSuccessors_recursive(chID, chID == mainSuccessor);
		}
	}

	vec3f Nodes::normalize(const vec3f& limit) {
		return normalize(limit, {0.0f, limit.y * -0.5f, 0.0f});
	}

	vec3f Nodes::normalize(vec3f limit, const vec3f& origin) {
		if (limit.x <= 0 || limit.y <= 0 || limit.z <= 0) __debugbreak(); // WTF size should be always positive!
		
		vec3f min;
		vec3f max;
		for (const tNode& node : nodes) {
			min.x = node.position.x < min.x ? node.position.x : min.x;
			min.y = node.position.y < min.y ? node.position.y : min.y;
			min.z = node.position.z < min.z ? node.position.z : min.z;
			
			max.x = node.position.x > max.x ? node.position.x : max.x;
			max.y = node.position.y > max.y ? node.position.y : max.y;
			max.z = node.position.z > max.z ? node.position.z : max.z;
		}

		{
			vec3f realSize = max - min;
			float biggestDimension = realSize.x > realSize.y ? (realSize.x > realSize.z ? realSize.x : realSize.z) : (realSize.y > realSize.z ? realSize.y : realSize.z);
			vec3f ratio = realSize / biggestDimension;
			limit *= ratio;
		}
		limit *= 0.5f;

		for (tNode& node : nodes) {
			node.position.x = remapf(node.position.x, min.x, max.x, -limit.x - origin.x, limit.x - origin.x);
			node.position.y = remapf(node.position.y, min.y, max.y, -limit.y - origin.y, limit.y - origin.y);
			node.position.z = remapf(node.position.z, min.x, max.z, -limit.z - origin.z, limit.z - origin.z);
		}

		return limit * 2;
	}
}