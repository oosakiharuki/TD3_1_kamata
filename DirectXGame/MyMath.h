#pragma once
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include <cmath>

using namespace KamataEngine;

struct AABB {
	Vector3 min;
	Vector3 max;
};

namespace MyMath {

	Vector3 operator+(const Vector3& v1, const Vector3& v2);
	Vector3 operator-(const Vector3& v1, const Vector3& v2);
	Vector3 operator*(const Vector3& v1, const Vector3& v2);
	Vector3 operator/(const Vector3& v1, const Vector3& v2);

	Vector3 operator*(const Vector3& v, const float f);
	Vector3 operator*(const float f, const Vector3& v);

	Vector3& operator+=(Vector3& v1, const Vector3& v2);
	Vector3& operator-=(Vector3& v1, const Vector3& v2);
	Vector3& operator*=(Vector3& v1, const Vector3& v2);
	Vector3& operator/=(Vector3& v1, const Vector3& v2);

	Matrix4x4 MakeIdentity4x4();
	Matrix4x4 MakeScaleMatrix(Vector3 scale);
	Matrix4x4 MakeRotateXMatrix(float radian);
	Matrix4x4 MakeRotateYMatrix(float radian);
	Matrix4x4 MakeRotateZMatrix(float radian);
	Matrix4x4 MakeTranslateMatrix(Vector3 translate);

	Vector3 Normalize(const Vector3& v);

	#pragma region Affine

	Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	#pragma endregion

	#pragma region 逆数
	Matrix4x4 Inverse(const Matrix4x4& m);
	#pragma endregion

	Matrix4x4 MakePerspectiveFovMatrix(float forY, float aspectRatio, float nearClip, float farClip);

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	AABB CreateAABB(const Vector3& translate,const Vector3& size);

} // namespace MyMath
