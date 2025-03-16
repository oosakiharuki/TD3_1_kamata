#pragma once
#include "Ground.h"
#include "KamataEngine.h"
#include "Player.h"

#include "Block.h"
//=======
#include "Enemy.h"

#include "CannonEnemy.h"


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

	uint32_t textureHandle = 0;


//<<<<<<< ステージギミック
	Block* block_ = nullptr;
	Model* modelBlock_ = nullptr;

//=======
	CannonEnemy* cannonEenmy = nullptr;
};

