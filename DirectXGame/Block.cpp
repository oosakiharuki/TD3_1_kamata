#include "Block.h"

void Block::Init(Model* model, Camera* viewProjection) {
    model_ = model;
    viewProjection_ = viewProjection;

    worldTransform.Initialize();
}

void Block::Update() {}

void Block::Draw() {
    model_->Draw(worldTransform, *viewProjection_);
}