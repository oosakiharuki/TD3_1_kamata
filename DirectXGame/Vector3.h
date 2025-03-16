#pragma once
#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

class Vector3 {
public:
	float x, y, z;

	// コンストラクタ
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}


	// 加算
	Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }

	// 減算
	Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }

	// スカラー乗算
	Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

	// スカラー除算
	Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

	// 内積
	float dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }

	// 外積
	Vector3 cross(const Vector3& v) const { return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }

	// ベクトルの長さ
	float length() const { return std::sqrt(x * x + y * y + z * z); }

	// 正規化
	Vector3 normalize() const {
		float len = length();
		if (len == 0)
			return Vector3(0, 0, 0);
		return *this / len;
	}

	// 等価演算子
	bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }

	// 等価でない演算子
	bool operator!=(const Vector3& v) const { return !(*this == v); }
};

#endif // VECTOR3_H
