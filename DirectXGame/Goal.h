#pragma once
#include "3d/WorldTransform.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "2d/Sprite.h"
#include "AABB.h"

using namespace KamataEngine;

class Goal{
public:

	Goal();
	~Goal();

	void Init(Model* model,Camera* camera, Vector3 position);
	void Update();
	void Draw();
	void Text();

	void OnCollision();
	bool IsClear() const { return isClear; }

	AABB GetAABB();

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;

	uint32_t textureHandle = 0;
	Sprite* sprite = nullptr;

	bool isClear = false;
};