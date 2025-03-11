#pragma once
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

class Enemy {
public:
	Enemy();
	~Enemy();

	void Init(Camera* camera);
	void Update();
	void Draw();

	// 障害物リスト（AABB）の設定／追加
	void SetObstacleList(const std::vector<AABB>& obstacles);
	void AddObstacle(const AABB& obstacle);

	// AABBを取得するメソッドを追加
	AABB GetAABB() const;
	
	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }
	void ContralPlayer();
	void ReMove(const Vector3& position_);
	bool GetPlayerCtrl() { return isPlayer; }

	const Vector3& GetWorldTranslate() { return worldTransform_.translation_; }

	// 位置を設定するメソッドを追加
	void SetPosition(const Vector3& position);


private:
	WorldTransform worldTransform_; // Fix the error by ensuring the type is defined
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	Vector3 position = {0, 0, -20};
	bool onGround_ = true;
	float velocityY_ = 0.0f;

	// 障害物リスト
	std::vector<AABB> obstacleList_;

	//WorldTransform worldTransform;
	Model* model_ = nullptr;

	Vector3 velocity;
	//bool IsJump = false;

	XINPUT_STATE state, preState;
	const float speed = 0.2f;

	// 移動
	bool Normal = true;
	float timer = 0.0f;
	const float corveTime = 1.0f;

	// スタン
	bool isStan = false;
	float timerS = 0.0f;
	const float stanTime = 3.0f;

	bool isPlayer = false;
};
