#include "Goal.h"
#include "Mymath.h"
#include <base/TextureManager.h>
#include <numbers>

Goal::Goal() {}
Goal::~Goal() {
	delete sprite;
	delete model_;
}

void Goal::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();

	// モデルの読み込み
	model_ = Model::CreateFromOBJ("goal", true);

	// 勝利画面用テクスチャの読み込み
	textureHandle = TextureManager::Load("winScene.png");

	// スプライトの作成 - nullチェックを追加
	if (textureHandle != 0) {
		sprite = Sprite::Create(textureHandle, {0, 0});
	} else {
		sprite = nullptr; // 明示的にnullに設定
	}

	// 行列を更新
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}


void Goal::Update() {
	if (isClear) {

		if (Input::GetInstance()->TriggerKey(DIK_R)) {
			isClear = false;
		}
#ifdef _DEBUG
		ImGui::Begin("Restart");
		ImGui::Text("keyBorad 'R' Restart");
		ImGui::End();
#endif // _DEBUG
	}
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void Goal::Draw() { model_->Draw(worldTransform_, *camera_); }

void Goal::Text() {
	// isClearがtrueでスプライトが正常に初期化されている場合のみ描画
	if (isClear && sprite != nullptr) {
		sprite->Draw();
	}
}

void Goal::OnCollision() { isClear = true; }

AABB Goal::GetAABB() {
	float halfW = 2.0f, halfH = 2.0f, halfD = 2.0f;
	AABB aabb;
	aabb.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	aabb.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};
	return aabb;
}
