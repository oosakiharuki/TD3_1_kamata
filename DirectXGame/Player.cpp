#include "Player.h"
#include <cassert>
#include <algorithm>


void Player::Init(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& pos) {

	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = pos;
	//input_ = KamataEngine::Input::GetInstance();
	worldTransform_.Initialize();

}

// ワールド座標を取得
KamataEngine::Vector3 Player::GetWorldPosition() { 

	// ワールド座標を入れる変数
	KamataEngine::Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {

	worldTransform_.parent_ = parent;

}

void Player::Update() {

	// キャラクターの移動ベクトル
	KamataEngine::Vector3 move = {0, 0, 0};
	

	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;


	worldTransform_.UpdateMatarix();

}

void Player::Draw() { 

	model_->Draw(worldTransform_, *camera_);

}