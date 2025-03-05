#pragma once
#include <algorithm>

namespace KamataEngine {

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