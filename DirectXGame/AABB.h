#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

struct AABB {
    Vector3 min;
    Vector3 max;
};

bool IsCollisionAABB(const AABB& a, const AABB& b);