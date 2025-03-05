#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "AABB.h"
#include "CameraController.h"
#include "Collision.h"
#include "math/Vector3.h"
#include <vector>

using namespace KamataEngine;

class Player {
public:
	Player();
	~Player();

	void Init(Camera* camera);
	void Update();
	void Draw();

	// 障害物リスト（AABB）の設定／追加
	void SetObstacleList(const std::vector<AABB>& obstacles);
	void AddObstacle(const AABB& obstacle);

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	Vector3 position = {0, 0, 0};
	bool onGround_ = true;
	float velocityY_ = 0.0f;
	CameraController cameraController_;

	// 障害物リスト
	std::vector<AABB> obstacleList_;
};
