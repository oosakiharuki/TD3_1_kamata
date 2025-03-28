#include "Bom.h"

Bom::Bom() {}
Bom::~Bom() { delete model_; }

void Bom::Init(Vector3 position, Vector3 velocity) {
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	model_ = Model::CreateFromOBJ("EnemyBullet", true);
	position_ = position;
	worldTransform_.translation_ = position_;

	velocity_ = velocity;
}

void Bom::Update() { 
	//const float Speed = 0.5f;

	position_.x += velocity_.x;
	position_.y += velocity_.y;
	position_.z += velocity_.z;	
	
	worldTransform_.translation_ = position_;

	worldTransform_.UpdateMatrix();

	deadTimer -= 1.0f / 60.0f;

	if (deadTimer < 0) {
		isDead = true;
	}
}

void Bom::Draw(Camera* camera) {

	model_->Draw(worldTransform_,*camera);

}

AABB Bom::GetAABB() {
	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f; 
	AABB bomAABB;
	bomAABB.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	bomAABB.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};

	return bomAABB;
}

void Bom::OnCollision() { isDead = true; }
