#pragma once
#include "KamataEngine.h"
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <btBulletDynamicsCommon.h>

using namespace KamataEngine;

class Ground {
public:
	// コンストラクタとデストラクタ
	Ground() = default;
	~Ground();

	// 初期化処理。引数としてカメラのポインタを受け取ります。
	void Init(Camera* camera);

	// 更新処理。ワールドトランスフォームの更新を行います。
	void Update();

	// 描画処理。読み込んだモデルを描画します。
	void Draw();

	// 物理オブジェクト取得用
	btRigidBody* GetRigidBody() { return rigidBody_; }

private:
	WorldTransform worldTransform;
	Camera* camera_ = nullptr;
	Model* groundModel_ = nullptr;
	Vector3 position;

	// Bullet 物理オブジェクト
	btTriangleMesh* triangleMesh_ = nullptr;
	btBvhTriangleMeshShape* meshShape_ = nullptr;
	btRigidBody* rigidBody_ = nullptr;
	btDefaultMotionState* motionState_ = nullptr;

	// モデルから三角メッシュを作成するヘルパー関数
	void CreateTriangleMeshFromModel();
};