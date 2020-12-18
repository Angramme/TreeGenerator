#pragma once

#include <ctime>

#include "TreeCrown.h"
#include "TreeGenerator.h"
#include "TreeVector.h"

#include <Urho3D/Graphics/Model.h>

namespace Tree {

	enum class CrownType {
		BOX, CIRCLE, OVAL
	};

	struct tParameters {
		CrownType crown_type;
		unsigned int crown_point_count;
		vec3f crown_center;
		vec3f crown_size; // use x for r in Circle | x,y,z for size in box | x, y for r1, r2 and z for rotation in oval | etc...
		
		float gen_max_dist;
		float gen_min_dist;
		float branch_length;
		std::vector<vec3f> trunks;

		unsigned int mdl_decimate_count;
		bool mdl_smooth;
		float mdl_min_R;
		float mdl_n_R;
		unsigned int mdl_min_vertex_count;
		unsigned int mdl_max_vertex_count;

		vec3f BBsize; //bounding-box size
	};

	Urho3D::SharedPtr<Urho3D::Model> tModel(Urho3D::Context * context_, const Tree::Generator & gen);
	Urho3D::SharedPtr<Urho3D::Model> tModel(Urho3D::Context * context_, const Tree::Generator & gen, const Tree::tParameters & params);


	Urho3D::SharedPtr<Urho3D::Model> getUrhoModelFromParams(Urho3D::Context* context_, const tParameters& params);
}