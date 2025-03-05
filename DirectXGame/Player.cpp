#include "Player.h"
#include <KamataEngine.h>

Player::Player() {}

Player::~Player() { delete PlayerModel_; }

void Player::Init(Camera* camera) {
	camera_ = camera;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);

	// プレイヤー初期位置（ワールド座標）を反映
	worldTransform_.translation_ = position;
}

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

	// ▼ 3) 重力処理と地面判定
	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	if (position.y < 0.0f) {
		position.y = 0.0f;
		velocityY_ = 0.0f;
		onGround_ = true;
	}

	// ▼ 4) プレイヤーのワールドトランスフォーム更新
	// 現在のプレイヤー位置をそのまま反映（代入により座標が累積しないように）
	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

	// ▼ 5) カメラコントローラを用いてカメラを追従させる
	cameraController_.Update(camera_, position);
}

void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }
