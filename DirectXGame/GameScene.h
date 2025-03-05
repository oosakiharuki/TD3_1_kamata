#pragma once
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;

class GameScene {

public:
	// ゲームシーン

	// コンストラクタ
	GameScene();

	// デストラクタ
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	Model* model_ = nullptr;

	WorldTransform worldTransform_;
	Camera camera_;
	//uint32_t texture = 0;

	Player* player_ = nullptr;
	Model* modelPlayer_ = nullptr;

};