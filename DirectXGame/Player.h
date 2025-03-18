#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"

#include "Mymath.h"

#include "3d/WorldTransform.h"
#include "AABB.h"
#include "CameraController.h"
#include "CannonEnemy.h"
#include "Collision.h"
#include "Enemy.h"
#include "input/input.h"

#include "Block.h"  // 衝突判定用にブロックをインクルード
#include <2d/ImGuiManager.h> // ImGuiのヘッダーを追加

#include "math/Vector3.h"
#include "SpringEnemy.h"
#include <vector>

class Enemy;
using namespace KamataEngine;

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

	enum class State {
		Normal, // 通常状態
		Bomb,   // ブロックを壊せる状態
		Ghost   // ブロックをすり抜ける状態
	};

	void DrawUI(); // UI描画用の関数を追加

	void SetBlock(Block* block) { block_ = block; }

	void CheckCollision(); // 衝突判定を追加

private:
////<<<<<<< ステージギミック
//    //WorldTransform worldTransform;
//    //Camera* viewProjection_ = nullptr;
//    //Model* model_ = nullptr;
//    Block* block_ = nullptr;  // 衝突判定用のブロックを保持
//
//    State currentState = State::Normal; // 初期状態をNormalに設定
//
//   // Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // 速度ベクトル
//    //Vector3 position;                      // 現在の位置
//    //bool IsJump = false;
//
//   //XINPUT_STATE state, preState;
//   // const float speed = 0.2f; // 移動速度
////};
////=======
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

	uint32_t textureHandle = 0;
	bool collisionEnemy = false;

	Vector3 stop = {0, 0, 0};

	Enemy* enemy = nullptr;

	Block* block_ = nullptr; // 衝突判定用のブロックを保持
	State currentState = State::Normal; // 初期状態をNormalに設定

	CannonEnemy* cannonEnemy = nullptr;	

	//float cameraPitch = 30.0f;
	float cameraYaw = 0.0f;

	bool isOpenDoor = false;
	AABB doorAABB;
	std::vector<SpringEnemy*> springEnemies_;
};
