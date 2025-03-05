#include "Player.h"
#include <KamataEngine.h>

void Player::Init(Camera* camera) {

	camera_ = camera;

	worldTranform.Initialize();
	PlayerModel_=Model::CreateFromOBJ("cube", true);

	 // プレイヤー初期位置を適宜指定
	position = {0, 0, 0};
}

void Player::Update() {
	// ▼ 1) WASD 入力による水平移動
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

	// ▼ 2) スペースキーによるジャンプ (簡易実装)
	//    onGround_ が true なら着地状態とみなし、スペースキーで上方向に速度を与える
	if (Input::GetInstance()->PushKey(DIK_SPACE) && onGround_) {
		velocityY_ = 0.3f; // ジャンプ力(要調整)
		onGround_ = false;
	}

	// ▼ 3) 重力をかける & 地面判定
	float gravity = 0.01f;
	velocityY_ -= gravity;    // 下向きに加速
	position.y += velocityY_; // プレイヤーのY座標を更新

	// 地面を Y=0 として判定し、着地処理
	if (position.y < 0.0f) {
		position.y = 0.0f; // 地面より下に行かないようクリップ
		velocityY_ = 0.0f; // 下向き速度をリセット
		onGround_ = true;  // 着地フラグ
	}

	// ▼ 4) ワールドトランスフォームに反映
	worldTranform.translation_ = position;
	worldTranform.TransferMatrix();

	// ▼ 5) カメラ追従 (上から見下ろす視点)
	//    例：プレイヤーを中心に、Y+10, Z-20 くらい後方から追う形
	camera_->translation_.x = position.x;
	camera_->translation_.y = position.y + 10.0f;
	camera_->translation_.z = position.z - 20.0f;

	// 上から見下ろす角度を設定 (X軸回転)
	float pitchDeg = 20.0f; // お好みの角度に変更可能
	camera_->rotation_.x = pitchDeg * (3.14159265f / 180.0f);
	camera_->rotation_.y = 0.0f;
	camera_->rotation_.z = 0.0f;

	// カメラ行列を更新 & 転送
	camera_->UpdateViewMatrix();
	camera_->TransferMatrix();
}

void Player::Draw() {PlayerModel_->Draw(worldTranform, *camera_); }
