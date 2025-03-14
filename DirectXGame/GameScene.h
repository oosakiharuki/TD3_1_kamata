#pragma once
#include "Enemy.h"
#include "Ground.h"
#include "KamataEngine.h"
#include "MapChipField.h" // MapChipField の定義が含まれる
#include "MapChipRenderer.h"
#include "Player.h"
using namespace KamataEngine;

class GameScene {
public:
	GameScene();
	~GameScene();
	void Initialize();
	void Update();
	void Draw();
	void SpawnEnemy(const Vector3& position);

private:
	void AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max);

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;
	Player* player_ = nullptr;
	Ground* modelGround_ = nullptr;

	// 既存の TileMap から MapChipField へ変更
	MapChipField* mapChipField_ = nullptr;
	// マップチップ描画用のクラス（MapChipField とモデル読み込み・描画処理を一括管理）
	MapChipRenderer* mapChipRenderer_ = nullptr;

	// Enemy リストおよび床障害物（床はそのまま）
	std::vector<Enemy*> enemyList_;
	std::vector<std::vector<AABB>> floorObstacles_;

	uint32_t textureHandle = 0;
};
