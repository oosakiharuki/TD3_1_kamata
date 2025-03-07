#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

class Block {
public:
	void Init(Model* model, Camera* viewProjection);
	void Update();
	void Draw();

private:
	WorldTransform worldTransform;
	Camera* viewProjection_ = nullptr;
	Model* model_ = nullptr;
};