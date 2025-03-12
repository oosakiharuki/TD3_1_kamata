#pragma once
#include <btBulletDynamicsCommon.h>

class BulletPhysicsManager {
public:
	static BulletPhysicsManager* GetInstance() {
		static BulletPhysicsManager instance;
		return &instance;
	}

	// デフォルトで 1/60 秒刻みで更新
	void Update(float deltaTime = 1.0f / 60.0f);
	void AddRigidBody(btRigidBody* body);
	void RemoveRigidBody(btRigidBody* body);

	// CollisionObject 用の関数
	void AddCollisionObject(btCollisionObject* obj);
	void RemoveCollisionObject(btCollisionObject* obj);
	btCollisionWorld* GetCollisionWorld();

	// デバッグ描画
	void DebugDraw();

private:
	BulletPhysicsManager();
	~BulletPhysicsManager();

	btDefaultCollisionConfiguration* collisionConfig_ = nullptr;
	btCollisionDispatcher* dispatcher_ = nullptr;
	btBroadphaseInterface* broadphase_ = nullptr;
	btSequentialImpulseConstraintSolver* solver_ = nullptr;
	btDiscreteDynamicsWorld* dynamicsWorld_ = nullptr;
};