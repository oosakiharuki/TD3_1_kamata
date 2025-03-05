#include "AABB.h"

AABB::AABB() : min_(Vector3(0, 0, 0)), max_(Vector3(0, 0, 0)) {}

AABB::AABB(const Vector3& min, const Vector3& max) : min_(min), max_(max) {}

bool AABB::CheckCollision(const AABB& other) const {
    return (min_.x <= other.max_.x && max_.x >= other.min_.x) &&
        (min_.y <= other.max_.y && max_.y >= other.min_.y) &&
        (min_.z <= other.max_.z && max_.z >= other.min_.z);
}