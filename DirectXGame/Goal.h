#pragma once
#include "2d/Sprite.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "AABB.h"

using namespace KamataEngine;

class Goal {
public:
	Goal();
	~Goal();

	void Init(Camera* camera);
	void Update();
	void Draw();
	void Text();

	void OnCollision();
	bool IsClear() const { return isClear; }

	AABB GetAABB();

	// 位置を設定するメソッドを追加
	void SetPosition(const Vector3& position) {
		worldTransform_.translation_ = position;
		worldTransform_.TransferMatrix();
		worldTransform_.UpdateMatrix();
	}

private:

	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;

	uint32_t textureHandle = 0;
	Sprite* sprite = nullptr;

	bool isClear = false;
};