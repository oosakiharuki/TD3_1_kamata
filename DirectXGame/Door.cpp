#include "Door.h"
#ifdef _DEBUG
#include "imgui.h"
#endif

Door::Door() {}

Door::~Door() { delete model_; }

void Door::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();

	// "cube" モデルを読み込み
	model_ = Model::CreateFromOBJ("door", true);

	// 行列を更新
	worldTransform_.UpdateMatrix();
}

// すべてのキーが取得されているかチェックするヘルパーメソッド
bool Door::AreAllKeysObtained() const {
	// キーが設定されていない場合は開かない
	if (keys_.empty()) {
		return false;
	}

	// 取得済みのキーの数をカウント
	int obtainedCount = 0;
	for (const auto* key : keys_) {
		if (key && key->IsKeyObtained()) {
			obtainedCount++;
		}
	}

	// 指定された数以上のキーが取得されているかチェック
	// requiredKeyCount_が設定されていれば、その数を使用
	// そうでなければ、すべてのキーが必要
	int required = (requiredKeyCount_ > 0) ? requiredKeyCount_ : static_cast<int>(keys_.size());
	return obtainedCount >= required;
}

void Door::Update() {
	// プレイヤーとの衝突判定
	if (player_) {
		AABB playerAABB = player_->GetAABB();
		AABB doorAABB = GetAABB();

		// プレイヤーにもAABBを渡す
		player_->ResolveCollisionWithDoor(doorAABB);

		if (IsCollisionAABB(playerAABB, doorAABB) && !isDoorOpened_) {
			// すべてのキーが取得されている場合、ドアに触れたフラグを立ててアニメーション開始
			if (AreAllKeysObtained() && !isDoorTouched_) {
				isDoorTouched_ = true;
				isAnimating_ = true;
			}
		}
		player_->SetOpenDoor(isDoorOpened_);
	}

	// ドアの開閉アニメーション
	if (isAnimating_) {
		// ドアを開く（Y軸回転：約90度＝1.5ラジアン）
		if (openAngle_ < 1.5f) {
			openAngle_ += 0.05f;
			worldTransform_.rotation_.y = openAngle_;

			if (openAngle_ >= 1.5f) {
				isDoorOpened_ = true;
				isAnimating_ = false;
			}
		}
	}

	// 行列更新
	worldTransform_.UpdateMatrix();
#ifdef _DEBUG
	// ImGuiによるデバッグ表示
	ImGui::Begin("Door Status");
	ImGui::Checkbox("Door Touch", &isDoorTouched_);
	ImGui::Checkbox("Door Opened", &isDoorOpened_);

	// キーの状態も表示
	int totalKeys = static_cast<int>(keys_.size());
	int obtainedKeys = 0;
	for (const auto* key : keys_) {
		if (key && key->IsKeyObtained()) {
			obtainedKeys++;
		}
	}

	ImGui::Text("Keys: %d/%d (Required: %d)", obtainedKeys, totalKeys, requiredKeyCount_);
	ImGui::End();
#endif
}

void Door::Draw() { model_->Draw(worldTransform_, *camera_); }

AABB Door::GetAABB() const {
	// ドアのローカル座標：
	//   min = (-3, 0, -1), max = (3, 6, 1)
	//   中心 = (0, 3, 0)
	//   半サイズ(=halfExtents) = (3, 3, 1)
	Vector3 localCenter = {0.0f, 3.0f, 0.0f};
	Vector3 halfExtents = {3.0f, 3.0f, 1.0f};

	// スケールを要素ごとに乗算
	Vector3 scaledCenter = {localCenter.x * worldTransform_.scale_.x, localCenter.y * worldTransform_.scale_.y, localCenter.z * worldTransform_.scale_.z};

	Vector3 scaledHalfExtents = {halfExtents.x * worldTransform_.scale_.x, halfExtents.y * worldTransform_.scale_.y, halfExtents.z * worldTransform_.scale_.z};

	// ドアのワールド中心 = ピボット位置(translation_) + scaledCenter
	Vector3 doorCenter = {worldTransform_.translation_.x + scaledCenter.x, worldTransform_.translation_.y + scaledCenter.y, worldTransform_.translation_.z + scaledCenter.z};

	// AABBを計算
	AABB doorAABB;
	doorAABB.min = {doorCenter.x - scaledHalfExtents.x, doorCenter.y - scaledHalfExtents.y, doorCenter.z - scaledHalfExtents.z};
	doorAABB.max = {doorCenter.x + scaledHalfExtents.x, doorCenter.y + scaledHalfExtents.y, doorCenter.z + scaledHalfExtents.z};
	return doorAABB;
}