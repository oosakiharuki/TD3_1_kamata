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

	// 重複宣言を削除し、ここに1つだけ残す
	void SetCannon(CannonEnemy* cannon);

	void OnCollisions();

	void ResolveCollisionWithDoor(const AABB& aabb) { doorAABB = aabb; }
	void SetOpenDoor(bool isOpen) { isOpenDoor = isOpen; }

	void SetSpringEnemies(const std::vector<SpringEnemy*>& springEnemies);
	// ここに重複していた宣言を削除
	void CheckCollisionWithSprings();
	void SetBlocks(const std::vector<Block*> blocks) { blocks_ = blocks; }

	void SetGoal(Goal* goal) { goal_ = goal; }
	void CheckCollisionWithGoal();

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

private:
	Vector3 position = {0, 10, -10};
	Vector3 velocity;
	Vector3 size = {2, 2, 2};
	Vector3 stop = {0, 0, 0};

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
	
	float velocityY_ = 0.0f;
	float hp = 3;
	float coolTime = 0.0f;
	float cameraPitch = 5.0f;
	float cameraYaw = 0.0f;
	const float speed = 0.2f;
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

	std::vector<AABB> obstacleList_;
	std::vector<SpringEnemy*> springEnemies_;
	std::vector<Block*> blocks_;
};