#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

// AABB 構造体に障害物識別用の id メンバを追加
// id: 0 → 床（フロア）、1,2 → 障害物（□）として描画
struct AABB {
	Vector3 min;
	Vector3 max;
	int id;
};

bool IsCollisionAABB(const AABB& a, const AABB& b);
