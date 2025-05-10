#pragma once

#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace KamataEngine {

namespace MathUtility {

const float PI = 3.141592654f;

//--------------------------------------------------------
//  【 Vector2 の演算子など】
//--------------------------------------------------------

// ※単項演算子 +, - は衝突する可能性があるため省略
// 2項演算子オーバーロード例（必要に応じて定義）
// inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
//     return { v1.x + v2.x, v1.y + v2.y };
// }

//--------------------------------------------------------
//  【 Vector3 の演算子など】
//--------------------------------------------------------

// ※単項演算子 +, - は省略して衝突を回避
// 2項演算子オーバーロード（必要なものだけ）
// inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
// return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
// }
// inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
// return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
// }

//--------------------------------------------------------
//  【 Dot / Cross の実装 】 (リンクエラー対策)
//--------------------------------------------------------
inline float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

inline Vector3 Cross(const Vector3& v1, const Vector3& v2) { return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x}; }

//--------------------------------------------------------
//  【 行列関連 】
//--------------------------------------------------------

// 単位行列を求める例
inline Matrix4x4 MakeIdentityMatrix() {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
}

// 必要に応じて各種行列演算を追加...

} // namespace MathUtility

} // namespace KamataEngine
