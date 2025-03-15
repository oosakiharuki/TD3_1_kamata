#include "Door.h"
#include "imgui.h"

Door::Door() {}

Door::~Door() { delete model_; }

void Door::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();

	// "cube" モデルを読み込み
	model_ = Model::CreateFromOBJ("cube", true);

	// 位置を設定
	worldTransform_.translation_ = position_;

	// サイズを設定（ドアらしく縦長に）
	worldTransform_.scale_ = doorSize_;

	// 行列を更新
	worldTransform_.UpdateMatrix();
}

void Door::Update() {
	// プレイヤーとの衝突判定
	if (player_ && key_) {
		AABB playerAABB = player_->GetAABB();
		AABB doorAABB = GetAABB();

		if (IsCollisionAABB(playerAABB, doorAABB)) {
			// 鍵を持っている場合、ドアに触れたフラグを立てる
			if (key_->IsKeyObtained() && !isDoorTouched_) {
				isDoorTouched_ = true;
				isAnimating_ = true;
			}
		}
	}

	// ドアの開閉アニメーション
	if (isAnimating_) {
		// ドアを開く（Y軸回転）
		if (openAngle_ < 1.5f) { // 約90度（ラジアン）
			openAngle_ += 0.05f;
			worldTransform_.rotation_.y = openAngle_;

			if (openAngle_ >= 1.5f) {
				isDoorOpened_ = true;
				isAnimating_ = false;
			}
		}
	}

	// 行列を更新
	worldTransform_.UpdateMatrix();

	// ImGuiデバッグ表示
	ImGui::Begin("Door Status");
	ImGui::Checkbox("Door Touch", &isDoorTouched_);
	ImGui::Checkbox("Door Opened", &isDoorOpened_);
	ImGui::End();
}

void Door::Draw() { model_->Draw(worldTransform_, *camera_); }

// AABBを取得するメソッド
AABB Door::GetAABB() const {
	float halfW = 0.5f * worldTransform_.scale_.x;
	float halfH = 0.5f * worldTransform_.scale_.y;
	float halfD = 0.5f * worldTransform_.scale_.z;

	AABB doorAABB;
	doorAABB.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	doorAABB.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};

	return doorAABB;
}