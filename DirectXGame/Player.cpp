#include "Player.h"
#include <KamataEngine.h>

Player::Player() {}

Player::~Player() { delete PlayerModel_; }

void Player::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);
	// 初期位置をワールドトランスフォームに反映
	worldTransform_.translation_ = position;
}

// Groundオブジェクトを設定するメソッドの実装
void Player::SetGround(Ground* ground) { ground_ = ground; }

void Player::Update() {
	// ▼ 1) 入力によるプレイヤーの水平移動
	float moveSpeed = 0.5f;
	if (Input::GetInstance()->PushKey(DIK_W)) {
		position.z += moveSpeed; // 前進
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		position.z -= moveSpeed; // 後退
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		position.x -= moveSpeed; // 左
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		position.x += moveSpeed; // 右
	}

	// ▼ 2) ジャンプ処理
	if (Input::GetInstance()->PushKey(DIK_SPACE) && onGround_) {
		velocityY_ = 0.3f;
		onGround_ = false;
	}

	// ▼ 3) 重力処理
	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	// ▼ 4) 地面との当たり判定
	if (ground_ != nullptr) {
		float groundHeight = ground_->GetHeightAt(position);
		if (position.y < groundHeight) {
			position.y = groundHeight;
			velocityY_ = 0.0f;
			onGround_ = true;
		}
	} else {
		// ground_が未設定の場合は従来通り平面（y=0）での判定
		if (position.y < 0.0f) {
			position.y = 0.0f;
			velocityY_ = 0.0f;
			onGround_ = true;
		}
	}

	// ▼ 5) プレイヤーのワールドトランスフォーム更新
	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

	// ▼ 6) カメラの追従処理
	cameraController_.Update(camera_, position);
}

void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }
