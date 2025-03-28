#include "Block.h"

void Block::Init(Model* model, Camera* viewProjection, uint32_t texturehandle) {
    model_ = model;
    viewProjection_ = viewProjection;

    worldTransform.Initialize();
	worldTransform.translation_ = {-4, 2, 4};
	texturehandle_ = texturehandle;
}

void Block::Update() { worldTransform.UpdateMatrix(); }

void Block::Draw() {
    if (!isActive_) return; // 非アクティブなら描画しない
    model_->Draw(worldTransform, *viewProjection_,texturehandle_);
}

AABB Block::GetAABB() const {
    AABB aabb;
    aabb.min = worldTransform.translation_ - Vector3(1.0f, 1.0f, 1.0f);
    aabb.max = worldTransform.translation_ + Vector3(1.0f, 1.0f, 1.0f);
    return aabb;
}