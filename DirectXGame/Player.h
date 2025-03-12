#pragma once
#include"KamataEngine.h"
#include "CameraController.h"

#include <vector>

using namespace KamataEngine;

class Player {
public:
	Player();
	~Player();

	void Init(Camera* camera);
	void Update();
	void Draw();


private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	Vector3 position = {0, 0, 0};
	bool onGround_ = true;
	float velocityY_ = 0.0f;
	CameraController cameraController_;

};