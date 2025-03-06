#include "Test.h"

void Test::Init(Model* model, Camera* viewProjection) {

	model_ = model;
	viewProjection_ = viewProjection;

	worldTransform.Initialize();
	worldTransform.translation_ = {3, 0, 0};

	aabb = CreateAABB(worldTransform.translation_, size);
}

void Test::Update() {


	aabb = CreateAABB(worldTransform.translation_, size);
	worldTransform.UpdateMatrix();

}

void Test::Draw() { model_->Draw(worldTransform, *viewProjection_); }

void Test::ContralPlayer() {
	isPlayer = true; 
	worldTransform.translation_ = {0, -2, 0};
}

void Test::ReMove(const Vector3& position) {
	if (isPlayer) {	
		worldTransform.translation_.x = position.x;
		worldTransform.translation_.y = position.y - 2.0f;
		worldTransform.translation_.z = position.z;
		isPlayer = false;
	}	
	worldTransform.parent_ = nullptr;
}