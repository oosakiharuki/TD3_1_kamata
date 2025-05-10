#pragma once
//<<<<<<< ステージギミック
#include <algorithm>
//=======
#ifndef VECTOR3_H
#define VECTOR3_H


#include <cmath>

//<<<<<<< ステージギミック
    /// <summary>
    /// 3次元ベクトル
    /// </summary>
    struct Vector3 final {
        float x;
        float y;
        float z;

        // コンストラクタ
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        // 成分ごとの最小値を取得
        static Vector3 Min(const Vector3& a, const Vector3& b) {
            return Vector3(
                std::min(a.x, b.x),
                std::min(a.y, b.y),
                std::min(a.z, b.z)
            );
        }

        // 成分ごとの最大値を取得
        static Vector3 Max(const Vector3& a, const Vector3& b) {
            return Vector3(
                std::max(a.x, b.x),
                std::max(a.y, b.y),
                std::max(a.z, b.z)
            );
        }

        // 演算子オーバーロード
        Vector3 operator+(const Vector3& rhs) const {
            return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        Vector3 operator-(const Vector3& rhs) const {
            return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3 operator/(float scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        // 代入演算子オーバーロード
        Vector3& operator+=(const Vector3& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        Vector3& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3& operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }
    };

} // namespace KamataEngine
//=======
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
//>>>>>>> Enemy_Taiho
