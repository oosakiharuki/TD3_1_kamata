#pragma once
#include"KamataEngine.h"
#include <cmath>
using namespace KamataEngine;

// クォータニオンを表現する構造体
struct Quaternion {
	float w, x, y, z;

	// コンストラクタ（デフォルト）
	Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}

	// コンストラクタ（指定された成分で初期化）
	Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

	// 単位クォータニオンを作成
	static Quaternion Identity() { return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); }

	// ベクトルからクォータニオンを作成 (回転軸と角度)
	static Quaternion FromAxisAngle(const Vector3& axis, float angleRad) {
		float halfAngle = angleRad * 0.5f;
		float sinHalfAngle = sin(halfAngle);
		return Quaternion(cos(halfAngle), axis.x * sinHalfAngle, axis.y * sinHalfAngle, axis.z * sinHalfAngle);
	}

	// クォータニオンの積 (回転の合成)
	Quaternion operator*(const Quaternion& q) const {
		return Quaternion(w * q.w - x * q.x - y * q.y - z * q.z, w * q.x + x * q.w + y * q.z - z * q.y, w * q.y - x * q.z + y * q.w + z * q.x, w * q.z + x * q.y - y * q.x + z * q.w);
	}

	// クォータニオンとベクトルの積 (ベクトルを回転)
	Vector3 operator*(const Vector3& v) const {
		// クォータニオンとベクトルを掛けて、ベクトルを回転させる
		Quaternion qv(0, v.x, v.y, v.z);
		Quaternion conj = this->Conjugate();
		Quaternion result = (*this) * qv * conj;
		return Vector3(result.x, result.y, result.z);
	}

	// クォータニオンの共役を返す（逆回転用）
	Quaternion Conjugate() const { return Quaternion(w, -x, -y, -z); }

	// クォータニオンを回転行列に変換
	Matrix4x4 ToMatrix() const {
		Matrix4x4 mat;
		mat.m[0][0] = 1 - 2 * (y * y + z * z);
		mat.m[0][1] = 2 * (x * y - z * w);
		mat.m[0][2] = 2 * (x * z + y * w);
		mat.m[1][0] = 2 * (x * y + z * w);
		mat.m[1][1] = 1 - 2 * (x * x + z * z);
		mat.m[1][2] = 2 * (y * z - x * w);
		mat.m[2][0] = 2 * (x * z - y * w);
		mat.m[2][1] = 2 * (y * z + x * w);
		mat.m[2][2] = 1 - 2 * (x * x + y * y);
		return mat;
	}

	// クォータニオンを正規化
	void Normalize() {
		float magnitude = sqrt(w * w + x * x + y * y + z * z);
		if (magnitude > 0.0f) {
			w /= magnitude;
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
		}
	}

	// クォータニオンをオイラー角に変換
	static Vector3 ToEuler(const Quaternion& q) {
		Vector3 euler;

		// Roll (X軸)
		float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		euler.x = atan2f(sinr_cosp, cosr_cosp); // float対応のatan2fを使用

		// Pitch (Y軸)
		float sinp = 2.0f * (q.w * q.y - q.z * q.x);
		if (fabsf(sinp) >= 1.0f)                        // fabsfでfloat対応
			euler.y = copysignf(3.14159f / 2.0f, sinp); // M_PIの代わりに3.14159fを使用
		else
			euler.y = asinf(sinp); // float対応のasinfを使用

		// Yaw (Z軸)
		float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		euler.z = atan2f(siny_cosp, cosy_cosp); // float対応のatan2fを使用

		return euler;
	}

	// == 演算子のオーバーロード
	bool operator==(const Quaternion& other) const { return (x == other.x && y == other.y && z == other.z && w == other.w); }

	// != 演算子のオーバーロード
	bool operator!=(const Quaternion& other) const { return !(*this == other); }
};

Quaternion EulerToQuaternion(float pitch, float yaw, float roll);
// クォータニオンをオイラー角に変換する関数
inline Vector3 QuaternionToEuler(const Quaternion& q) { return Quaternion::ToEuler(q); }