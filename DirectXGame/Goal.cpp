#include "Goal.h"
#include <base/TextureManager.h>
#include <numbers>
#include "Mymath.h"

Goal::Goal() {}
Goal::~Goal() { delete sprite; }

void Goal::Init(Model* model, Camera* camera, Vector3 position) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	textureHandle = TextureManager::Load("winScene.png");
	sprite  =Sprite::Create(textureHandle, {0, 0});
}

void Goal::Update() {
	if (isClear) {

		if (Input::GetInstance()->TriggerKey(DIK_R)) {
			isClear = false;
		}
		ImGui::Begin("Restart");
		ImGui::Text("keyBorad 'R' Restart");
		ImGui::End();
	}
	worldTransform_.UpdateMatrix();
}

void Goal::Draw() { model_->Draw(worldTransform_, *camera_); }

void Goal::Text() { 
	if (isClear)
	sprite->Draw(); 
}

void Goal::OnCollision() {
	isClear = true;
}

AABB Goal::GetAABB() {
	float halfW = 2.0f, halfH = 2.0f, halfD = 2.0f;
	AABB aabb;
	aabb.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	aabb.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};
	return aabb;
}
