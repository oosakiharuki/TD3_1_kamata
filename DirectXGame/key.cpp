
#include "Key.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

Key::Key() {}

Key::~Key() { delete model_; }

void Key::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	keyGTAudio_= Audio::GetInstance();
	// "cube" モデルを読み込み
	model_ = Model::CreateFromOBJ("key", true);

	// 位置を設定
	worldTransform_.translation_ = position_;

	// サイズを小さめに設定（見た目調整用）
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};

	// 行列を更新
	worldTransform_.UpdateMatrix();

	// 鍵取得音の読み込み
	KeyAudioHandle_ = keyGTAudio_->LoadWave("./sound/key_get.wav");

}

void Key::Update() {
	// 既に取得されていたら処理しない
	if (isObtained_) {
		return;
	}

	// プレイヤーとの衝突判定
	if (player_) {
		AABB playerAABB = player_->GetAABB();
		AABB keyAABB = GetAABB();

		if (IsCollisionAABB(playerAABB, keyAABB)) {
			// 衝突したら鍵を取得
			isObtained_ = true;

			// 鍵取得音を再生
			keyGTAudio_->playAudio(KeyGetAudio_, KeyAudioHandle_, false, 0.5);
		}
	}

	// 回転アニメーション
	if (!isObtained_) {
		rotationY_ += 0.02f;
		worldTransform_.rotation_.y = rotationY_;
	}

	// 行列を更新
	worldTransform_.UpdateMatrix();

	// ImGuiデバッグ表示
#ifdef _DEBUG
	ImGui::Begin("Key Status");
	ImGui::Text("Key ID: %d", keyID_);
	ImGui::Checkbox("KeyFlg", &isObtained_);
	ImGui::End();
#endif
}

void Key::Draw() {
	// 取得されていない場合のみ描画
	if (!isObtained_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

// AABBを取得するメソッド
AABB Key::GetAABB() const {
	float halfW = 0.5f * worldTransform_.scale_.x;
	float halfH = 0.5f * worldTransform_.scale_.y;
	float halfD = 0.5f * worldTransform_.scale_.z;

	AABB keyAABB;
	keyAABB.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	keyAABB.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};

	return keyAABB;
}