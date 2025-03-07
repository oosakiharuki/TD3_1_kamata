#pragma once
#include "3d/WorldTransform.h"
#include "MyMath.h"
#include "3d/Camera.h"
#include "3d/Model.h"

using namespace KamataEngine;
using namespace MyMath;

class Test{
public:
	void Init(Model* model, Camera* viewProjection);
	void Update();
	void Draw();
	AABB GetAABB() { return aabb; }

	void SetParent(const WorldTransform* parent) { worldTransform.parent_ = parent; }
	void ContralPlayer();
	void ReMove(const Vector3& position);
	bool GetPlayerCtrl() { return isPlayer; }

	Vector3 GetWorldTranslate() { return worldTransform.translation_; }

private:
	WorldTransform worldTransform;
	Camera* viewProjection_ = nullptr;
	Model* model_ = nullptr;

	Vector3 size = {2, 2, 2};
	AABB aabb;

	bool isPlayer = false;

};