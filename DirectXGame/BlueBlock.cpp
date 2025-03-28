#include "BlueBlock.h"

void BlueBlock::Init(Model* model, Camera* viewProjection, uint32_t texturehandle) {
    model_ = model;
    viewProjection_ = viewProjection;

    worldTransform.Initialize();
    worldTransform.translation_ = { 4, 2, 0 };
    texturehandle_ = texturehandle;
}

void BlueBlock::Update() {
    worldTransform.UpdateMatrix();
}

void BlueBlock::Draw() {
    if (!isActive_) return; // 非アクティブなら描画しない
    model_->Draw(worldTransform, *viewProjection_, texturehandle_);
}

AABB BlueBlock::GetAABB() const {
    AABB aabb;
    aabb.min = worldTransform.translation_ - Vector3(1.0f, 1.0f, 1.0f);
    aabb.max = worldTransform.translation_ + Vector3(1.0f, 1.0f, 1.0f);
    return aabb;
}