#pragma once
#include "Block.h"
#include "CannonEnemy.h"
#include "Door.h"
#include "Enemy.h"
#include "Ground.h"
#include "KamataEngine.h"
#include "Key.h"
#include "MapLoader.h" // 追加：MapLoaderをインクルード
#include "Player.h"
#include "SpringEnemy.h"

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

	void SpawnEnemy(const Vector3& position); // 新しいメソッドを追加

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

	CannonEnemy* cannonEnemy = nullptr;

	Model* stage = nullptr;
	std::stringstream Command;

	// 追加：MapLoaderのインスタンス
	MapLoader* mapLoader_ = nullptr;

	Block* block_ = nullptr;
	Model* modelBlock_ = nullptr;
};