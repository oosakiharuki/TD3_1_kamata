#include "Player.h"

void Player::Init(Model* model, Camera* viewProjection) { 

	model_ = model;
	viewProjection_ = viewProjection;

	worldTranform.Initialize();
}

void Player::Update() { 
	worldTranform.TransferMatrix();
}

void Player::Draw() { 
	model_->Draw(worldTranform,*viewProjection_);
}


