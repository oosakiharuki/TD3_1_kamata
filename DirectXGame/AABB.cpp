#include "AABB.h"

// AABB の衝突判定（軸平行境界ボックス同士）
bool IsCollisionAABB(const AABB& a, const AABB& b) {
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) && // x軸
	    (a.min.y <= b.max.y && a.max.y >= b.min.y) && // y軸
	    (a.min.z <= b.max.z && a.max.z >= b.min.z)) { // z軸
		return true;
	}
	return false;
}
