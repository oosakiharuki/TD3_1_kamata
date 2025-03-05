#pragma once

#include "KamataEngine.h"
#include "Player.h"
#include "RailCamera.h"
#include <sstream>
using namespace KamataEngine;

class GameScene {
public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void Draw();

	int32_t timer = 0;
	bool timerflag = true;

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	Player* player_ = nullptr;
	Model* modelSkydome_ = nullptr;
	RailCamera* railCamera_ = nullptr;

	Vector3 RailCamerPos = {0, 0, 0};
	Vector3 RailCamerRad = {0, 0, 0};

	Model* modelPlayer_ = nullptr;
	Model* modelEnemy_ = nullptr;

	WorldTransform worldTransform_;
	Camera camera_;

	Vector3 railcameraPos = {0, 0, 0};
	Vector3 railcameraRad = {0, 0, 0};

	// オーディオ関連のメンバ変数
	int hitSoundHandle_ = 0;
	int hitSound_ = -1;
	uint32_t texturtitle_ = 0;
	KamataEngine::Sprite* title1_ = nullptr;

	uint32_t texturtitle2_ = 0;
	KamataEngine::Sprite* title12_ = nullptr;

	uint32_t texturtitle3_ = 0;
	KamataEngine::Sprite* title13_ = nullptr;

	Vector3 playerPos = {0, 0, 30};
};