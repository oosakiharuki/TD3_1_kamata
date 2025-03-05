#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;

class AABB {
public:
    Vector3 min_;  // 名前変更
    Vector3 max_;  // 名前変更

    AABB();
    AABB(const Vector3& min, const Vector3& max);

    bool CheckCollision(const AABB& other) const;
};