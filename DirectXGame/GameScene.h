#pragma once
#include "CannonEnemy.h"
#include "Enemy.h"
#include "Ground.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Door.h"
#include "Key.h"
#include "SpringEnemy.h"

using namespace KamataEngine;

class GameScene {
public:
	GameScene();
	~GameScene();
	void Initialize();
	void Update();
	void Draw();

private:
	void AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max);

	void LoadStage(std::string objFile);

	void UpdateStageAABB();

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;
	Player* player_ = nullptr;
	Ground* modelGround_ = nullptr;

	// Enemyのリストを追加
	std::vector<Enemy*> enemyList_;
	std::vector<std::vector<AABB>> allObstacles_;

	// 追加：ばね敵のリスト
	std::vector<SpringEnemy*> springEnemies_;

	uint32_t textureHandle = 0;

	CannonEnemy* cannonEenmy = nullptr;

	Model* stage = nullptr;
	std::stringstream Command;

	Key* key_ = nullptr;   // 鍵オブジェクト
	Door* door_ = nullptr; // ドアオブジェクト
};