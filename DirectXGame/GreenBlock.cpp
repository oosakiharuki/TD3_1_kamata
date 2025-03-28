#include "GreenBlock.h"

void GreenBlock::Init(Model* model, Camera* viewProjection, uint32_t texturehandle) {
    model_ = model;
    viewProjection_ = viewProjection;

    worldTransform.Initialize();
    worldTransform.translation_ = { 4, 2, -4 };
    texturehandle_ = texturehandle;
}

void GreenBlock::Update() {
    worldTransform.UpdateMatrix();
}

void GreenBlock::Draw() {
    if (!isActive_) return; // 非アクティブなら描画しない
    model_->Draw(worldTransform, *viewProjection_, texturehandle_);
}

AABB GreenBlock::GetAABB() const {
    AABB aabb;
    aabb.min = worldTransform.translation_ - Vector3(1.0f, 1.0f, 1.0f);
    aabb.max = worldTransform.translation_ + Vector3(1.0f, 1.0f, 1.0f);
    return aabb;
}