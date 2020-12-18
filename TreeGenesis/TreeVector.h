#pragma once

#include <iostream>
#include <math.h>

static float Q_rsqrt(const float& number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = *(long*)&y; // evil floating point bit level hacking

	i = 0x5f3759df - (i >> 1); // WTF?!

	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y)); // 1st iteration
	//y = y * (threehalfs - (x2 * y * y)); // 2nd iteration, this can be removed
	
	return y;
}

namespace Tree {
	template<typename T>
	class mat3x4;

	template<typename T>
	class vec3 {
	public:
		T x, y, z;

	public:
		vec3()
			: x(0), y(0), z(0)
		{}

		vec3(T x, T y, T z)
			: x(x), y(y), z(z)
		{}

		vec3(const vec3& v)
			: x(v.x), y(v.y), z(v.z)
		{}

		vec3(T* v)
			: x(v[0]), y(v[1]), z(v[2])
		{}

		inline T distSQ(const vec3& other) const {
			return (x - other.x)*(x - other.x) + (y - other.y)*(y - other.y) + (z - other.z)*(z - other.z);
		}
		inline T dist(const vec3& other) const {
			return Q_rsqrt(distSQ(other));
		}

		inline T magnitude() const {
			return Q_rsqrt(x * x + y * y + z * z);
		}

		inline vec3& normalize() {
			if (x == 0 && y == 0 && z == 0) return *this; // NO! bad vector!

			T r_length = magnitude();
			x *= r_length;
			y *= r_length;
			z *= r_length;

			return *this;
		}

		inline vec3 cross(const vec3& other) const {
			return vec3(
				y * other.z - other.y * z,
				other.x * z - x * other.z,
				x * other.y - other.x * y
			);
		}

		inline vec3 operator+(const vec3& other) const {
			return vec3(
				this->x + other.x,
				this->y + other.y,
				this->z + other.z
			);
		}

		inline void operator+=(const vec3& other) {
			x += other.x;
			y += other.y;
			z += other.z;
		}

		inline vec3 operator-(const vec3& other) const {
			return vec3(
				this->x - other.x,
				this->y - other.y,
				this->z - other.z
			);
		}

		inline vec3 operator*(const T& other) const {
			return vec3(
				x * other,
				y * other,
				z * other
			);
		}

		inline vec3& operator*=(const T& other) {
			x *= other;
			y *= other;
			z *= other;
			return *this;
		}

		inline vec3& operator*=(const vec3& other) {
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		inline vec3 operator*(const mat3x4<T>& m) const {
			return vec3(
				x * m.col1.x + y * m.col2.x + z * m.col3.x + m.col4.x,
				x * m.col1.y + y * m.col2.y + z * m.col3.y + m.col4.y,
				x * m.col1.z + y * m.col2.z + z * m.col3.z + m.col4.z
			);
		}

		inline vec3& operator *=(const mat3x4<T>& m) {
			*this = operator*(m);
			return *this;
		}

		inline vec3 operator/(const T& other) const {
			return vec3(
				x / other,
				y / other,
				z / other
			);
		}

		inline vec3& operator/=(const T& other) {
			x /= other;
			y /= other;
			z /= other;
			return *this;
		}

		static vec3 random() {
			return vec3(
				(float)(rand() % 10000) / 5000.0f - 1.0f,
				(float)(rand() % 10000) / 5000.0f - 1.0f,
				(float)(rand() % 10000) / 5000.0f - 1.0f
			);
		}
	};

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const vec3<T>& v) {
		o << "vec3<" << typeid(T).name() << "> { " << v.x << "," << v.y << "," << v.z << " }";
		return o;
	}

	typedef vec3<float> vec3f;
	typedef vec3<int> vec3i;
	typedef vec3<unsigned short> vec3us;


	/////////////////////


	template<typename T>
	class vec2 {
	public:
		T x, y;

	public:
		vec2()
			: x(0), y(0)
		{}

		vec2(T x, T y)
			: x(x), y(y)
		{}

		vec2(const vec2& v)
			: x(v.x), y(v.y)
		{}

		vec2(T* v)
			: x(v[0]), y(v[1])
		{}
	};

	typedef vec2<float> vec2f;
	typedef vec2<int> vec2i;
	typedef vec2<unsigned short> vec2us;

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const vec2<T>& v) {
		o << "vec2<" << typeid(T).name() << "> { " << v.x << "," << v.y << " }";
		return o;
	}
}