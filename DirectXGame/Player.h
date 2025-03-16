#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"

#include "3d/WorldTransform.h"
#include "AABB.h"
#include "CameraController.h"
#include "Collision.h"
#include "math/Vector3.h"
#include <vector>
#include "Enemy.h"
#include "input/input.h"
//#include "Block.h"  // 衝突判定用にブロックをインクルード
//#include "AABB.h"
//#include <2d/ImGuiManager.h> // ImGuiのヘッダーを追加

#include "CannonEnemy.h"

using namespace KamataEngine;

enum class Controler { 
	player, 
	enemyTransfar 
};

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

	// 障害物リスト（AABB）の設定／追加
	void SetObstacleList(const std::vector<AABB>& obstacles);
	void AddObstacle(const AABB& obstacle);

	// Enemyのリストを設定するメソッドを追加
	void SetEnemyList(const std::vector<Enemy*>& enemies);

	// Enemyのリストを追加
	std::vector<Enemy*> enemyList_;

	void EnemyHead() { onEnemy = true; }

	void SetCannon(CannonEnemy* cannon) { cannonEnemy = cannon; }

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
	Vector3 position = {0, 0, -10};
	bool onGround_ = true;
	float velocityY_ = 0.0f;
	CameraController cameraController_;

	// 障害物リスト
	std::vector<AABB> obstacleList_;

	// WorldTransform worldTransform;
	//Camera* viewProjection_ = nullptr;
	//Model* model_ = nullptr;

	Vector3 velocity;
	bool onEnemy;
	bool isTransfar = false; // のりうつる体制

	XINPUT_STATE state, preState;
	const float speed = 0.2f;

	// AABB aabb;
	AABB playerAABB;
	Vector3 size = {2, 2, 2};

	AABB enemyAABB;

	bool EnemyContral = false;

	Controler controler = Controler::player;
	uint32_t textureHandle = 0;
	bool collisionEnemy = false;


	CannonEnemy* cannonEnemy = nullptr;

	float cameraPitch = 0.0f;
	float cameraYaw = 0.0f;
};
