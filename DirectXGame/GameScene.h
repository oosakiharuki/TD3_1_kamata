#pragma once
#include "Block.h"
#include "RedBlock.h"
#include "BlueBlock.h"
#include "GreenBlock.h"

#include "EnemyLoader.h"
#include "CannonEnemy.h"
#include "SpringEnemy.h"

#include "Ground.h"
#include "Door.h"
#include "Key.h"
#include "Goal.h"

#include "MapLoader.h" // MapLoaderをインクルード
#include "Player.h"
#include "Skydome.h"

#include "KamataEngine.h"

using namespace KamataEngine;

class GameScene {
public:
	// ゲームシーン

	// コンストラクタ
	GameScene();

	// デストラクタ
	~GameScene();
	void Finalize();

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


	// 障害物リスト
	std::vector<std::vector<AABB>> allObstacles_;

	uint32_t textureHandle = 0;

	Model* stage = nullptr;
	std::stringstream Command;

	// MapLoaderのインスタンス
	MapLoader* mapLoader_ = nullptr;

	// EnemyLoaderのインスタンス
	EnemyLoader* enemyLoader_ = nullptr;

	Block* block_ = nullptr;
	Model* modelBlock_ = nullptr;

	RedBlock* redBlock_ = nullptr;
	Model* modelRedBlock_ = nullptr;

	BlueBlock* blueBlock_ = nullptr;
	Model* modelBlueBlock_ = nullptr;

	GreenBlock* greenBlock_ = nullptr;
	Model* modelGreenBlock_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	float longPress = 1.0f;
	const float RestartTimer = 1.0f;
	XINPUT_STATE state, preState;


	Goal* goal = nullptr;
	Model* modelGoal_ = nullptr;
};