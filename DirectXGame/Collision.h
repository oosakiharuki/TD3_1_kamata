#pragma once
#include "AABB.h"
#include "math/Vector3.h"
#include <cmath>

// 各軸の重なり量を計算する関数
inline Vector3 GetOverlapAmount(const AABB& a, const AABB& b) {
	float overlapX1 = b.max.x - a.min.x;
	float overlapX2 = a.max.x - b.min.x;
	float overlapX = (overlapX1 < overlapX2) ? overlapX1 : overlapX2;

	float overlapY1 = b.max.y - a.min.y;
	float overlapY2 = a.max.y - b.min.y;
	float overlapY = (overlapY1 < overlapY2) ? overlapY1 : overlapY2;

	float overlapZ1 = b.max.z - a.min.z;
	float overlapZ2 = a.max.z - b.min.z;
	float overlapZ = (overlapZ1 < overlapZ2) ? overlapZ1 : overlapZ2;

	return {overlapX, overlapY, overlapZ};
}

// 衝突している場合、最小の重なり軸方向へ押し戻す処理
// velocityY：プレイヤーの垂直速度、onGround：着地判定
inline void ResolveAABBCollision(AABB& playerAABB, const AABB& obstacleAABB, float& velocityY, bool& onGround) {
	Vector3 overlap = GetOverlapAmount(playerAABB, obstacleAABB);

	// 重なりが最小の軸で押し戻しを行う
	if (overlap.x < overlap.y && overlap.x < overlap.z) {
		float playerCenterX = (playerAABB.min.x + playerAABB.max.x) * 0.5f;
		float obstacleCenterX = (obstacleAABB.min.x + obstacleAABB.max.x) * 0.5f;
		float push = (playerCenterX < obstacleCenterX) ? -overlap.x : overlap.x;
		playerAABB.min.x += push;
		playerAABB.max.x += push;
	} else if (overlap.y < overlap.x && overlap.y < overlap.z) {
		float playerCenterY = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
		float obstacleCenterY = (obstacleAABB.min.y + obstacleAABB.max.y) * 0.5f;
		float push = (playerCenterY < obstacleCenterY) ? -overlap.y : overlap.y;
		playerAABB.min.y += push;
		playerAABB.max.y += push;
		// 上向きの押し戻しなら着地判定を立てる
		if (push > 0.0f) {
			velocityY = 0.0f;
			onGround = true;
		}
	} else if (overlap.z < overlap.x && overlap.z < overlap.y) {
		float playerCenterZ = (playerAABB.min.z + playerAABB.max.z) * 0.5f;
		float obstacleCenterZ = (obstacleAABB.min.z + obstacleAABB.max.z) * 0.5f;
		float push = (playerCenterZ < obstacleCenterZ) ? -overlap.z : overlap.z;
		playerAABB.min.z += push;
		playerAABB.max.z += push;
	}
}
