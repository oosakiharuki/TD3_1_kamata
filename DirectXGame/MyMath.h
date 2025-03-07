#pragma once
#include"KamataEngine.h"
#include "AABB.h"

using namespace KamataEngine;

// アフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale_, const Vector3& rotation_, const Vector3& translation_);

// 行列の掛け算
Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float angle);

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float angle);

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float angle);

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

Vector3 Normalize(const Vector3& v);

AABB CreateAABB(const Vector3& translate,const Vector3& size);
