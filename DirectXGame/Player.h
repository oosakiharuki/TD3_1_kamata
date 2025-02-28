#pragma once
#include "3d/WorldTransform.h"
#include "math/Vector3.h"
#include "3d/Camera.h"
#include "3d/Model.h"

using namespace KamataEngine;

class Player {
public:
	void Init(Model* model,Camera* viewProjection);
	void Update();
	void Draw();

private:
	WorldTransform worldTranform;
	Camera* viewProjection_ = nullptr;
	Model* model_ = nullptr;

	Vector3 position;
};