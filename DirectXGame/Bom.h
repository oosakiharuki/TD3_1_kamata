#pragma once
#include "3d/WorldTransform.h"
#include "AABB.h"
#include "collision.h"

#include "Mymath.h"

using namespace KamataEngine;

class Bom{
public:
	Bom();
	~Bom();
	void Init(Vector3 position,Vector3 velocity);
	void Update();
	void Draw(Camera* camera);
	bool IsDaed() { return isDead; }

	AABB GetAABB();

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Vector3 position_;
	bool onGround_ = true;

	Vector3 velocity_;
	float deadTimer = 3.0f;
	bool isDead = false;
};