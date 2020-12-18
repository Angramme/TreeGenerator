#include "Tree.h"

using namespace Tree;
Urho3D::SharedPtr<Urho3D::Model> Tree::getUrhoModelFromParams(Urho3D::Context* context_, const tParameters& params) {
	srand(std::time(nullptr));
	//srand(555);

	Tree::Crown treecrown;
	switch (params.crown_type) {
	case CrownType::BOX:
		treecrown = Tree::BoxCrown(params.crown_point_count, params.crown_center, params.crown_size);
		break;
	case CrownType::CIRCLE:
		treecrown = Tree::CircularCrown(params.crown_point_count, params.crown_center, params.crown_size.x);
		break;
	case CrownType::OVAL:
		treecrown = Tree::OvalCrown(params.crown_point_count, params.crown_center, params.crown_size.x, params.crown_size.y, params.crown_size.z);
		break;
	}
	std::cout << "got the crown!" << std::endl;
	Tree::Generator gen(treecrown, params.gen_max_dist, params.gen_min_dist, params.branch_length);
	for (const vec3f& trunk : params.trunks) {
		gen.add_trunk(trunk);
		std::cout << "got the trunk at: " << trunk << std::endl;
	}

	std::cout << "starting generation!" << std::endl;
	gen.processAll();

	// TODO: add gravity

	unsigned Nbranches = gen.branches.size();

	std::cout << "finished generation! " << Nbranches << " branches" << std::endl;

	Urho3D::SharedPtr<Urho3D::Model> treemodel = Tree::tModel(context_, gen, params);
	return treemodel;
}