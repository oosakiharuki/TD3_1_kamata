#include "Player.h"
#include <KamataEngine.h>

Player::Player() {}

Player::~Player() {

	delete PlayerModel_; }

void Player::Init(Camera* camera) {
	camera_ = camera;

	// ワールドトランスフォーム初期化
	worldTranform.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);

	// プレイヤー初期位置（ワールド座標）
	position = {0, 0, 0};
}

void Player::Update() {
	// ▼ 1) WASD 入力によるプレイヤーの水平移動
	float moveSpeed = 0.2f;
	if (Input::GetInstance()->PushKey(DIK_W)) {
		position.z += moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		position.z -= moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		position.x -= moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		position.x += moveSpeed;
	}

	// ▼ 2) スペースキーによるジャンプ（簡易実装）
	if (Input::GetInstance()->PushKey(DIK_SPACE) && onGround_) {
		velocityY_ = 0.3f; // ジャンプ力（必要に応じて調整）
		onGround_ = false;
	}

	// ▼ 3) 重力処理と地面判定（地面：Y=0 と仮定）
	float gravity = 0.01f;
	velocityY_ -= gravity;    // 下向きに加速
	position.y += velocityY_; // Y座標更新

	if (position.y < 0.0f) {
		position.y = 0.0f;
		velocityY_ = 0.0f;
		onGround_ = true;
	}

	// ▼ 4) プレイヤーのワールドトランスフォームに反映
	worldTranform.translation_ = position;
	worldTranform.TransferMatrix();

	// ▼ 5) カメラ追従設定
	// プレイヤーを常に画面中央に表示するため、カメラの位置はプレイヤーの座標に対して一定のオフセットを与えます
	camera_->translation_.x = position.x;
	camera_->translation_.y = position.y + 10.0f; // Y軸は少し高めに配置
	camera_->translation_.z = position.z - 20.0f; // Z軸はプレイヤーから離す

	// カメラの向き設定（上から見下ろす角度）
	float pitchDeg = 20.0f; // 必要に応じて変更可能
	camera_->rotation_.x = pitchDeg * (3.14159265f / 180.0f);
	camera_->rotation_.y = 0.0f;
	camera_->rotation_.z = 0.0f;

	// カメラ行列を更新＆転送
	camera_->UpdateViewMatrix();
	camera_->TransferMatrix();
}

void Player::Draw() { PlayerModel_->Draw(worldTranform, *camera_); }