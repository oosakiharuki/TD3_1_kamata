#include "Skydome.h"

void Skydome::Initialize(Model* model, Camera* camera) {

	assert(model);
	model_ = model;

	worldTransform_.Initialize();
	camera_ = camera;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
}

void Skydome::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	worldTransform_.rotation_.y += 0.008f; // 回転速度は調整可能
	// 行列計算
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw() {
	// 3Dモデル描画
	model_->Draw(worldTransform_, *camera_);
}