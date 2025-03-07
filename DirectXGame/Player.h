#pragma once
#include "3d/WorldTransform.h"
#include "MyMath.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "input/input.h"

using namespace KamataEngine;
using namespace MyMath;

enum class Controler {
	player,
	enemyTransfar
};



class Player {
public:
	void Init(Model* model,Camera* viewProjection);
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

private:
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