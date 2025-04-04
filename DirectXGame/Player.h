#pragma once
#include "AABB.h"
#include "Block.h"
#include "CameraController.h"
#include "CannonEnemy.h"
#include "Collision.h"
#include "Enemy.h"
#include "GhostBlock.h"
#include "Goal.h"
#include "KamataEngine.h"
#include "Mymath.h"
#include "SpringEnemy.h"
#include "UIManager.h"
#include <vector>

class Enemy;
using namespace KamataEngine;

class Player {
public:
	Player();
	~Player();

	void Init(Camera* camera);
	void Update();
	void Draw();

	AABB GetAABB() { return playerAABB; }
	void IsOnEnemy(bool set) { onEnemy = set; }
	bool GetIsTransfar() { return isTransfar; }

	void GetEnemyHead(AABB aabb) { enemyAABB = aabb; }

	const WorldTransform* GetWorld() { return &worldTransform_; }
	Vector3 GetWorldPosition() { return position; }

	bool GetEnemyContral() { return EnemyContral; }
	void SetEnemyContral(bool anser) {
		isTransfar = false;
		velocity.y = 0.0f;
		EnemyContral = anser;
	}

	void SetObstacleList(const std::vector<AABB>& obstacles);
	void AddObstacle(const AABB& obstacle);

	void SetEnemyList(const std::vector<Enemy*>& enemies);

	std::vector<Enemy*> enemyList_;

	void EnemyHead() { onEnemy = true; }

	void SetPosition(const Vector3& position);

	void SetCannon(CannonEnemy* cannon);

	void OnCollisions();

	void ResolveCollisionWithDoor(const AABB& aabb) { doorAABB = aabb; }
	void SetOpenDoor(bool isOpen) { isOpenDoor = isOpen; }

	void SetSpringEnemies(const std::vector<SpringEnemy*>& springEnemies);
	void CheckCollisionWithSprings();
	void SetBlocks(const std::vector<Block*> blocks) { blocks_ = blocks; }

	void SetGoal(Goal* goal) { goal_ = goal; }
	void CheckCollisionWithGoal();
	bool IsGoalReached() const { return isGoalReached_; }
	void ResetGoalFlag() { isGoalReached_ = false; }

	enum class State {
		Normal, // 通常状態
		Bomb,   // ブロックを壊せる状態
		Ghost   // ブロックをすり抜ける状態
	};

	void DrawUI();
	void SetBlock(Block* block, GhostBlock* ghostBlock) {
		block_ = block;
		ghostBlock_ = ghostBlock;
	}

	void CheckCollision();
	void SetState(State newState);
	void ClearObstacleList();
	void CheckDamage();

	// HP関連の新機能
	int GetHP() const { return hp; }
	void TakeDamage(int amount);
	void CheckFallDamage();
	void CheckEnemyDamage();
	void ResetToSpawnPosition();
	void SetSpawnPosition(const Vector3& pos) { spawnPosition_ = pos; }
	Vector3 GetSpawnPosition() const { return spawnPosition_; }

private:
	Vector3 position = {0, 10, -10};
	Vector3 velocity;
	Vector3 size = {2, 2, 2};
	Vector3 stop = {0, 0, 0};
	Vector3 spawnPosition_ = {0, 10, -10}; // 初期スポーン位置

	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	CameraController cameraController_;

	bool onGround_ = true;
	bool onEnemy;
	bool isTransfar = false;
	bool isDamage = false;
	bool EnemyContral = false;
	bool collisionEnemy = false;
	bool isOpenDoor = false;
	bool isGoalReached_ = false;

	float velocityY_ = 0.0f;
	int hp = 200;                        // int型として保持
	const int maxHP_ = 200;              // int型として保持
	const int enemyDamage_ = 5;          // 敵からのダメージ
	const int fallDamage_ = 5;           // 落下ダメージ
	const float fallThreshold_ = -50.0f; // 落下判定のY座標

	float coolTime = 0.0f;
	float cameraPitch = 5.0f;
	float cameraYaw = 0.0f;
	const float speed = 0.28f;
	XINPUT_STATE state, preState;
	AABB playerAABB;
	AABB enemyAABB;
	AABB doorAABB;

	uint32_t textureHandle = 0;
	Enemy* enemy = nullptr;
	Block* block_ = nullptr;
	State currentState = State::Normal;
	CannonEnemy* cannonEnemy = nullptr;
	GhostBlock* ghostBlock_ = nullptr;
	Goal* goal_ = nullptr;

	// UI管理
	UIManager* uiManager_ = nullptr;

	std::vector<AABB> obstacleList_;
	std::vector<SpringEnemy*> springEnemies_;
	std::vector<Block*> blocks_;

	// 点滅関連の変数
	bool isFlashing = false;          // 点滅中かどうか
	float flashTimer = 0.0f;          // 点滅用タイマー
	bool isVisible = true;            // 現在表示中かどうか
	const float flashInterval = 0.1f; // 点滅間隔（秒）
	const float flashDuration = 1.0f; // 点滅継続時間（秒）

	// サウンド関連
	Audio* audio_ = nullptr;
	int jumpSoundHandle_ = 0;
	int jumpSoundID_ = -1;
	int snapSoundHandle_ = 0;
	int snapSoundID_ = -1;
	int damageSoundHandle_ = 0;
	int damageSoundID_ = -1;
};