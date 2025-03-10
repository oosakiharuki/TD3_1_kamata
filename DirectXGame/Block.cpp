#include "Block.h"

void Block::Init(Model* model, Camera* viewProjection) {
    model_ = model;
    viewProjection_ = viewProjection;

    worldTransform.Initialize();
}

void Block::Update() {}

void Block::Draw() {
    if (!isActive_) return; // 非アクティブなら描画しない
    model_->Draw(worldTransform, *viewProjection_);
}

AABB Block::GetAABB() const {
    AABB aabb;
    aabb.min = worldTransform.translation_ - Vector3(0.5f, 0.5f, 0.5f);
    aabb.max = worldTransform.translation_ + Vector3(0.5f, 0.5f, 0.5f);
    return aabb;
}