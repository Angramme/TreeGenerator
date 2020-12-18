#pragma once

#include <iostream>
//#include "TreeVector.h"

namespace Tree {
	template<typename T>
	class vec3;

	template<typename T>
	class mat3x4 {
	public:
		vec3<T> col1;
		vec3<T> col2;
		vec3<T> col3;
		vec3<T> col4;

		mat3x4() {
		}

		inline void translate(const vec3<T>& p) {
			col4 = p;
		}

		static mat3x4 from_direction(const vec3<T>& dir, const vec3<T>& up = vec3<T>(0,1,0)) {
			mat3x4 m;

			vec3<T> xaxis = up.cross(dir);
			xaxis.normalize();

			vec3<T> yaxis = dir.cross(xaxis);
			yaxis.normalize();

			m.col1 = xaxis;
			m.col2 = yaxis;
			m.col3 = dir;

			return m;
		}
	};

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const mat3x4<T>& m) {
		o << "mat3x4<" << typeid(T).name() << "> { " << std::endl
			<< m.col1.x << ", " << m.col2.x << ", " << m.col3.x << ", " << m.col4.x << ", " << std::endl
			<< m.col1.y << ", " << m.col2.y << ", " << m.col3.y << ", " << m.col4.y << ", " << std::endl
			<< m.col1.z << ", " << m.col2.z << ", " << m.col3.z << ", " << m.col4.z << std::endl << " } " << std::endl;
		return o;
	}

	typedef mat3x4<float> mat3x4f;
}