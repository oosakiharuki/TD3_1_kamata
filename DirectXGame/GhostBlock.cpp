#include "GhostBlock.h"

GhostBlock::GhostBlock() {}
GhostBlock::~GhostBlock() { delete model_; }

void GhostBlock::Init(Camera* camera_) {
    Camera_ = camera_;
	model_ = Model::CreateFromOBJ("cube", true);
    worldTransform.Initialize();
    worldTransform.translation_ = { 5, 2, 3 };
	worldTransform.TransferMatrix();
}

void GhostBlock::Update() {
    worldTransform.UpdateMatrix();
}

void GhostBlock::Draw() {
    if (!isActive_) return; // 非アクティブなら描画しない
    model_->Draw(worldTransform,*Camera_);
}

AABB GhostBlock::GetAABB() const {
    AABB aabb;
    aabb.min = worldTransform.translation_ - Vector3(1.0f, 1.0f, 1.0f);
    aabb.max = worldTransform.translation_ + Vector3(1.0f, 1.0f, 1.0f);
    return aabb;
}