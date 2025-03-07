#pragma once

#include "3d/WorldTransform.h"
#include "MyMath.h"

#include "3d/Camera.h"
#include "3d/Model.h"

#include "3d/WorldTransform.h"
#include "AABB.h"
#include "CameraController.h"
#include "Collision.h"
#include "math/Vector3.h"
#include <vector>

#include "input/input.h"


using namespace KamataEngine;
using namespace MyMath;

enum class Controler {
	player,
	enemyTransfar
};



class Player {
public:
	Player();
	~Player();

	void Init(Camera* camera);
	void Update();
	void Draw();
	AABB GetAABB() { return aabb; }
	void IsOnEnemy(bool set) { onEnemy = set; }
	bool GetIsTransfar() { return isTransfar; }

	void GetHead(float a) { yuka = a; }

	const WorldTransform* GetWorld() { return &worldTransform; }
	
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

private:

	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	Vector3 position = {0, 0, -10};
	bool onGround_ = true;
	float velocityY_ = 0.0f;
	CameraController cameraController_;

	// 障害物リスト
	std::vector<AABB> obstacleList_;

	WorldTransform worldTransform;
	Camera* viewProjection_ = nullptr;
	Model* model_ = nullptr;

	Vector3 position;
	Vector3 velocity;
	bool onGround;
	bool onEnemy;
	bool isTransfar = false;//のりうつる体制


	XINPUT_STATE state, preState;
	const float speed = 0.2f;

	AABB aabb;
	Vector3 size = {2, 2, 2};

	float yuka = 0.0f;

	bool EnemyContral = false;

	Controler controler = Controler::player;
};

