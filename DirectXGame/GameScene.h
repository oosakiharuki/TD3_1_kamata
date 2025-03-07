#pragma once
#include "Ground.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Test.h"

using namespace KamataEngine;

class GameScene {
public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void Draw();
	
	void Collision();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;

	Player* player_ = nullptr;

	Model* modelPlayer_ = nullptr;

	Test* test = nullptr;
	Model* modelT = nullptr;

	bool EnemyContral = false;

	Ground* modelGround_ = nullptr;
};

