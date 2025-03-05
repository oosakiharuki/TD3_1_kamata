#include "Player.h"

void Player::Init(Camera* camera) {

	camera_ = camera;

	worldTranform.Initialize();
	PlayerModel_=Model::CreateFromOBJ("cube", true);
}

void Player::Update() { worldTranform.TransferMatrix(); }

void Player::Draw() {PlayerModel_->Draw(worldTranform, *camera_); }
