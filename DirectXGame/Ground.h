#pragma once
#include"KamataEngine.h"
using namespace KamataEngine;

class Ground {
public:

	void Init(Camera* camera, Model* model);
	void Update();
	void Draw();

private:
	WorldTransform worldTransform;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;
};
