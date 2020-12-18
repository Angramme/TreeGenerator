#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D\Graphics\VertexBuffer.h>
#include <Urho3D\Graphics\IndexBuffer.h>

#include <vector>
#include <cmath>

//#include "TreeModel.h"
#include "Tree.h"

#include "TreeMatrix.h"
#include "TreeNode.h"

#include "TreeNodes.h"


static Urho3D::SharedPtr<Urho3D::Geometry> getTreeGeometry(std::vector<Tree::tNode>& nodes, Urho3D::Context* context_) {
	using namespace Urho3D;
	using namespace Tree;

	const unsigned numNodes = nodes.size();
	const unsigned numVertices = 3 * numNodes;

	struct tVertex {
		Tree::vec3f position, normal, tangent;
		Tree::vec2f uv;
	};

	std::vector<tVertex> vertexData;
	std::vector<Tree::vec3us> indexData;

	for (unsigned i = 0; i < numNodes; i++) {
		const tNode& node = nodes[i];
		const tNode& parent = nodes[node.parentID];
		const float& R = node.childrenID.size() >0 ? node.R : node.R *0.65;

		{ // vertex data
			const float PI2 = 3.14159265359 * 2;
			const float ang = PI2 / node.nV;
			const float dist_to_parent = node.position.dist(parent.position);

			const float UV_multiplier = node.pos_from_root % 2 == 0 ? dist_to_parent / (R*ang) : 3; // to avoid texture tretching because branches are longer than they are wide

			const Tree::vec3f& nD = node.direction;
			const Tree::mat3x4f rot = Tree::mat3x4f::from_direction(
				( nD.y == 0 && nD.z == 0 ? 
					Tree::vec3f(0.0f, 0.0f, 1.0f) : 
					Tree::vec3f(1.0f, 0.0f, 0.0f).cross(nD).normalize() ), // a bit ugly but it's the only solution that i found
				nD);


			for (unsigned j = 0; j < node.nV; j++) {
				float a = j * ang;
				Tree::vec3f norm(std::cosf(a) * R, 0.0f, std::sinf(a) * R);
				//Tree::vec3f tang(std::sinf(a) * R, 0.0f, std::cosf(a) * R);
				Tree::vec3f tang(0.0f, 1.0f, 0.0f);
				Tree::vec2f uv(
					(j ==node.nV -1 && node.nV%2 == 1)? 0.5 : (j%2==0? 0:1),
					node.pos_from_root%2==0? 0: UV_multiplier);
				norm *= rot;
				tang *= rot;

				vertexData.push_back({
					norm + node.position,
					norm,
					tang,
					uv
					});
			}

			if (!node.isMainSuccessor) {
				for (unsigned j = 0; j < node.nV; j++) {
					float a = j * ang;
					Tree::vec3f norm(std::cosf(a) * R, 0.0f, std::sinf(a) * R);
					//Tree::vec3f tang(std::sinf(a) * R, 0.0f, std::cosf(a) * R);
					Tree::vec3f tang(0.0f, 1.0f, 0.0f);
					Tree::vec2f uv(
						(j == node.nV - 1 && node.nV % 2 == 1) ? 0.5 : (j % 2 == 0 ? 0 : 1),
						node.pos_from_root % 2 == 0 ? UV_multiplier : 0);
					norm *= rot;
					tang *= rot;

					vertexData.push_back({
						norm + parent.position,
						norm,
						tang,
						uv
						});
				}
			}
		}
		/////////////////////////////////////////
		{ // index data
			// tip cap
			if (node.childrenID.size() == 0) {
				for (unsigned j = 1; j < node.nV - 1; j++) {
					indexData.push_back(Tree::vec3us( 0 + node.lV, j+1 + node.lV, j + node.lV ));
				}
			}

			tNode& node2 = nodes[node.parentID];

			// non-main successor connection with additional ring in parent
			if (!node.isMainSuccessor) {
				for (unsigned v1 = 0; v1 < node.nV; v1++) {
					unsigned v2 = (v1 + 1) % node.nV;

					indexData.push_back(Tree::vec3us(v1 + node.lV, v2 + node.lV, v1 + node.lV + node.nV));
					indexData.push_back(Tree::vec3us(v2 + node.lV, v2 + node.lV + node.nV, v1 + node.lV + node.nV));
				}
			}
			// continuous branch
			else if (node.nV == node2.nV) {
				for (unsigned v1 = 0; v1 < node.nV; v1++) {
					unsigned v2 = (v1 + 1) % node.nV;

					indexData.push_back(Tree::vec3us(v1+node.lV, v2+node.lV, v1+node2.lV));
					indexData.push_back(Tree::vec3us(v2+node.lV, v2+node2.lV, v1+node2.lV));
				}
			}
			// connection from different number of vertex per branch
			else {
				bool upwards = node.nV < node2.nV; // is branch size ( branch vertex count ) getting smaller upwards

				const tNode& sml = upwards ? node : node2;
				const tNode& big = upwards ? node2 : node;

				const float ratio = (float)(sml.nV-1) / (float)(big.nV-1);

				for (unsigned v1 = 0; v1 < big.nV; v1++) {
					const unsigned v2 = (v1 + 1) % big.nV; // vertex 2

					const unsigned v3 = std::roundf(v1 * ratio);
					const unsigned v4 = std::roundf(v2 * ratio);

					if (v3 == v4) {
						indexData.push_back(Tree::vec3us( v2 + big.lV, v1 + big.lV, v3 + sml.lV ));
					}
					else {
						indexData.push_back(Tree::vec3us( v2 + big.lV, v1 + big.lV, v3 + sml.lV ));
						indexData.push_back(Tree::vec3us( v2 + big.lV, v3 + sml.lV, v4 + sml.lV ));
					}
				}
			}
		}

	}

	SharedPtr<VertexBuffer> vb(new VertexBuffer(context_));
	vb->SetShadowed(true); // Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	PODVector<VertexElement> elements;
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_NORMAL));
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_TANGENT));
	elements.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD));
	vb->SetSize(vertexData.size(), elements);
	vb->SetData(static_cast<void*>(vertexData.data()));


	SharedPtr<IndexBuffer> ib(new IndexBuffer(context_));
	ib->SetShadowed(true);
	ib->SetSize(indexData.size() * 3, false);
	ib->SetData(static_cast<void*>(indexData.data()));


	SharedPtr<Geometry> geom(new Geometry(context_));
	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(TRIANGLE_LIST, 0, indexData.size() * 3);

	return geom;
}

using namespace Urho3D;
SharedPtr<Urho3D::Model> Tree::tModel(Urho3D::Context* context_, const Tree::Generator& gen)
{
	using namespace Tree;
	tParameters params = {
		CrownType::BOX, 42, {0,0,0}, {0,0,0},

		0, 0, 0, {},

		1, true, 0.2f, 2.5f, 3, 6, {50,50,50}
	};

	return tModel(context_, gen, params);
}
SharedPtr<Urho3D::Model> Tree::tModel(Urho3D::Context* context_, const Tree::Generator& gen, const Tree::tParameters& params)
{
	Tree::Nodes nodes(gen);

	for (unsigned i = 0; i < /*1*/ params.mdl_decimate_count; i++)
		nodes.decimate();
	// TODO meaby: add clean algorithm that deletes nodes that are too close ( chooses the one with bigger R )
	if(params.mdl_smooth) nodes.smooth();
	nodes.initR(/*0.2f, 2.5f*/ params.mdl_min_R, params.mdl_n_R);
	nodes.findSuccessors();
	nodes.initVNandVL(/*3,6*/ params.mdl_min_vertex_count, params.mdl_max_vertex_count);
	nodes.initDirection();
	Tree::vec3f BBsize = nodes.normalize(/*{50.0f, 50.0f, 50.0f}*/ params.BBsize);
	nodes.initPFR();

	SharedPtr<Geometry> geom = getTreeGeometry(nodes.nodes, context_); //FIX tipcap normals
	// TODO in treeGeom:
	// calculate UVs
	
	// TOADD: leaves geometry



	SharedPtr<Urho3D::Model> TreeModel(new Urho3D::Model(context_));
	TreeModel->SetNumGeometries(1);
	TreeModel->SetGeometry(0, 0, geom);
	TreeModel->SetBoundingBox(BoundingBox(
		Vector3( BBsize.x * -0.5f, 0.0f, BBsize.z * -0.5f ),
		Vector3( BBsize.x * 0.5f, BBsize.y, BBsize.z * 0.5f )
	));
	
	// Though not necessary to render, the vertex & index buffers must be listed in the model so that it can be saved properly
	Vector<SharedPtr<VertexBuffer> > vertexBuffers;
	Vector<SharedPtr<IndexBuffer> > indexBuffers;
	vertexBuffers.Push(SharedPtr<VertexBuffer>(geom->GetVertexBuffer(0)));
	indexBuffers.Push(SharedPtr<IndexBuffer>(geom->GetIndexBuffer()));
	// Morph ranges could also be not defined. Here we simply define a zero range (no morphing) for the vertex buffer
	PODVector<unsigned> morphRangeStarts;
	PODVector<unsigned> morphRangeCounts;
	morphRangeStarts.Push(0);
	morphRangeCounts.Push(0);
	TreeModel->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
	TreeModel->SetIndexBuffers(indexBuffers);
	
	return TreeModel;
}