#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "AABB.h"
#include "CameraController.h"
#include "CannonEnemy.h"
#include "Collision.h"
#include "Enemy.h"
#include "input/input.h"
#include "math/Vector3.h"
#include "SpringEnemy.h"
#include <vector>

using namespace KamataEngine;

enum class Controler { player, enemyTransfar };

class Player {
public:
	Player();
	~Player();

	void Init(Camera* camera, uint32_t texture);
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
		controler = Controler::enemyTransfar;
	}

	void SetObstacleList(const std::vector<AABB>& obstacles);
	void AddObstacle(const AABB& obstacle);

	void SetEnemyList(const std::vector<Enemy*>& enemies);

	std::vector<Enemy*> enemyList_;

	void EnemyHead() { onEnemy = true; }

	void SetCannon(CannonEnemy* cannon) { cannonEnemy = cannon; }

	// ★ 新しく追加：ドアとの衝突を解決するメソッド
	void ResolveCollisionWithDoor(const AABB& aabb) { doorAABB = aabb; }
	void SetOpenDoor(bool isOpen) { isOpenDoor = isOpen; }

	void SetSpringEnemies(const std::vector<SpringEnemy*>& springEnemies) { springEnemies_ = springEnemies; }
	void CheckCollisionWithSprings();

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	Vector3 position = {0, 10, -10};
	bool onGround_ = true;
	float velocityY_ = 0.0f;
	CameraController cameraController_;

	// 障害物リスト
	std::vector<AABB> obstacleList_;

	Vector3 velocity;
	bool onEnemy;
	bool isTransfar = false;

	XINPUT_STATE state, preState;
	const float speed = 0.2f;

	AABB playerAABB;
	Vector3 size = {2, 2, 2};

	AABB enemyAABB;

	bool EnemyContral = false;

	Controler controler = Controler::player;
	uint32_t textureHandle = 0;
	bool collisionEnemy = false;

	CannonEnemy* cannonEnemy = nullptr;

	bool isOpenDoor = false;
	AABB doorAABB;
	std::vector<SpringEnemy*> springEnemies_;
};
