#pragma once
#include "CameraController.h"
#include "KamataEngine.h"
#include <btBulletDynamicsCommon.h>

#include <vector>

using namespace KamataEngine;

// 衝突情報を保持するための構造体
struct ContactInfo {
	bool isContact = false;
	btVector3 contactNormal;
	btScalar contactDistance;
};

class Player {
public:
	Player();
	~Player();

	void Init(Camera* camera);
	void Update();
	void Draw();

	// 物理オブジェクト取得用
	btRigidBody* GetRigidBody() { return rigidBody_; }

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	Vector3 position = {0, 1, 0}; // 少し浮かせておく
	bool onGround_ = false;
	float velocityY_ = 0.0f;
	CameraController cameraController_;

	// 接触情報
	ContactInfo floorContact_; // 床との接触情報
	ContactInfo sideContact_;  // 側面との接触情報

	// Bullet 物理オブジェクト
	btCollisionShape* collisionShape_ = nullptr;
	btRigidBody* rigidBody_ = nullptr;
	btDefaultMotionState* motionState_ = nullptr;

	// フィールド範囲
	float fieldMinX;
	float fieldMaxX;
	float fieldMinZ;
	float fieldMaxZ;
	float fieldHeight;

	// 接地判定と側面接触判定を更新する関数
	void UpdateContactCheck();

	// 物理状態を同期する関数
	void SyncPhysicsTransform();
};