#include "TreeCrown.h"
#include <math.h>

namespace Tree {
	
	Crown::Crown()
	{}
	Crown::Crown(const unsigned int& numPoints)
	{
		points.reserve(numPoints);
	}
	Crown::~Crown()
	{}

	Crown CircularCrown(const unsigned int& numPoints, const vec3f& center, const unsigned int& r)
	{
		Crown crown(numPoints);

		for (int i = 0; i < numPoints; i++) {
			float angle = rand() % 360 / 180.0f * 3.14159265359f;
			float radius = rand() % (int)r;
			crown.points.push_back( vec3f(
				cos(angle)*radius + center.x,
				sin(angle)*radius + center.y,
				0 + center.z
			));
		}


		return crown;
	}

	Crown OvalCrown(const unsigned int& numPoints, const vec3f& center, const unsigned int& r1, const unsigned int& r2, const float& rotation)
	{
		Crown crown(numPoints);

		const float rDif = (int)r2 - (int)r1;

		for (int i = 0; i < numPoints; i++) {
			float angle = rand() % 360 / 180.0f * 3.14159265359f;
			float radius = rand() % (int)( r1 + abs(sin(angle+rotation))*rDif );
			crown.points.push_back( vec3f(
				cos(angle)*radius + center.x,
				sin(angle)*radius + center.y,
				0 + center.z
			));
		}

		return crown;
	}

	Crown BoxCrown(const unsigned& numPoints, const vec3f& center, const vec3f& size) {
		Crown crown(numPoints);
		
		for (unsigned i = 0; i < numPoints; i++) {
			crown.points.push_back(Tree::vec3f(
				center.x + std::fmodf( rand(), size.x ) - size.x * 0.5f,
				center.y + std::fmodf( rand(), size.y ) - size.y * 0.5f,
				center.z + std::fmodf( rand(), size.z ) - size.z * 0.5f
				));
		}

		return crown;
	}
}
