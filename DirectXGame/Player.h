#pragma once
#include"KamataEngine.h"
#include "Mymath.h"
#include "AABB.h"
#include "CameraController.h"
#include "CannonEnemy.h"
#include "Collision.h"
#include "Enemy.h"
#include "Block.h"  // 衝突判定用にブロックを
#include "GhostBlock.h"
#include "SpringEnemy.h"
#include <vector>
#include "Goal.h"
#include "RedGhost.h"
#include "BlueGhost.h"
#include "YellowGhost.h"

//class Enemy;
class RedGhost;
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

	void SetEnemyList(const std::vector<RedGhost*>& enemies);

	//std::vector<Enemy*> enemyList_;
	std::vector<RedGhost*> enemyList_;

	void EnemyHead() { onEnemy = true; }


	void SetCannon(CannonEnemy* cannon) { cannonEnemy = cannon; }
	
	void OnCollisions();

	// ★ 新しく追加：ドアとの衝突を解決するメソッド
	void ResolveCollisionWithDoor(const AABB& aabb) { doorAABB = aabb; }
	void SetOpenDoor(bool isOpen) { isOpenDoor = isOpen; }

	void SetSpringEnemies(const std::vector<SpringEnemy*>& springEnemies) { springEnemies_ = springEnemies; }
	void CheckCollisionWithSprings();

	void SetGoal(Goal* goal) { goal_ = goal; }
	void CheckCollisionWithGoal();

	enum class State {
		Normal, // 通常状態
		Bomb,   // ブロックを壊せる状態
		Ghost   // ブロックをすり抜ける状態
	};

	void DrawUI(); // UI描画用の関数を追加

	void SetBlock(Block* block, GhostBlock* ghostBlock) { 
		block_ = block; 
		ghostBlock_ = ghostBlock;
	}

	void CheckCollision(); // 衝突判定を追加

	void SetState(State newState);

	//ダメージをくらったクールタイム
	void CheckDamage();

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
	float hp = 3;
	bool isDamage = false;
	float coolTime = 0.0f;

	AABB enemyAABB;

	bool EnemyContral = false;

	uint32_t textureHandle = 0;
	bool collisionEnemy = false;

	Vector3 stop = {0, 0, 0};

	RedGhost* enemy = nullptr;

	Block* block_ = nullptr; // 衝突判定用のブロックを保持
	State currentState = State::Normal; // 初期状態をNormalに設定

	CannonEnemy* cannonEnemy = nullptr;	

	float cameraPitch = 5.0f;
	float cameraYaw = 0.0f;

	bool isOpenDoor = false;
	AABB doorAABB;
	std::vector<SpringEnemy*> springEnemies_;

	Goal* goal_ = nullptr;
	GhostBlock* ghostBlock_ = nullptr;
};
