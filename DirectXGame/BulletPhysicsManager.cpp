#include "BulletPhysicsManager.h"

BulletPhysicsManager::BulletPhysicsManager() {
	// Bullet 初期化
	collisionConfig_ = new btDefaultCollisionConfiguration();
	dispatcher_ = new btCollisionDispatcher(collisionConfig_);
	broadphase_ = new btDbvtBroadphase();
	solver_ = new btSequentialImpulseConstraintSolver();
	dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, collisionConfig_);

	// 重力設定
	dynamicsWorld_->setGravity(btVector3(0, -9.8f, 0));

	// 衝突処理の精度を上げる設定
	dynamicsWorld_->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
	dynamicsWorld_->getDispatchInfo().m_useContinuous = true;
}

BulletPhysicsManager::~BulletPhysicsManager() {
	delete dynamicsWorld_;
	delete solver_;
	delete broadphase_;
	delete dispatcher_;
	delete collisionConfig_;
}

void BulletPhysicsManager::Update(float deltaTime) {
	dynamicsWorld_->stepSimulation(deltaTime, 10); // 最大10サブステップで精度を上げる
}

void BulletPhysicsManager::AddRigidBody(btRigidBody* body) { dynamicsWorld_->addRigidBody(body); }

void BulletPhysicsManager::RemoveRigidBody(btRigidBody* body) { dynamicsWorld_->removeRigidBody(body); }

void BulletPhysicsManager::AddCollisionObject(btCollisionObject* obj) {
	// 衝突マージンを設定（めり込み防止）
	if (obj->getCollisionShape()) {
		obj->getCollisionShape()->setMargin(0.04f);
	}
	dynamicsWorld_->addCollisionObject(obj);
}

void BulletPhysicsManager::RemoveCollisionObject(btCollisionObject* obj) { dynamicsWorld_->removeCollisionObject(obj); }

btCollisionWorld* BulletPhysicsManager::GetCollisionWorld() { return dynamicsWorld_; }

void BulletPhysicsManager::DebugDraw() {
	// デバッグ描画が必要な場合は実装
}