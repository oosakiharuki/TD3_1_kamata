#pragma once
#include "AABB.h"
#include "math/Vector3.h"
#include <cmath>
#include <vector>

// 各軸の重なり量を計算する関数のプロトタイプ宣言
Vector3 GetOverlapAmount(const AABB& a, const AABB& b);

// 衝突している場合、最小の重なり軸方向へ押し戻す処理のプロトタイプ宣言
// velocityY：プレイヤーの垂直速度、onGround：着地判定
void ResolveAABBCollision(AABB& playerAABB, const AABB& obstacleAABB, float& velocityY, bool& onGround);

// プレイヤーとブロックとの衝突判定を行い、衝突している場合はプレイヤーAABBを修正する関数
void CheckPlayerBlockCollision(AABB& playerAABB, const std::vector<AABB>& blockObstacles, float& velocityY, bool& onGround);
