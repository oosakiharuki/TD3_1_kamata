#pragma once
#include "Ground.h"
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;

class GameScene {
public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void Draw();

private:

	void AddObstacle(std::vector<AABB>& obstacles, const Vector3& min, const Vector3& max);

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;

	Player* player_ = nullptr;
	Ground* modelGround_ = nullptr;
};
