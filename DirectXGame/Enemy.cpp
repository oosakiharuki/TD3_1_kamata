#include "Enemy.h"
#include <KamataEngine.h>
#include "AABB.h"
#include "Collision.h"

Enemy::Enemy() {}

Enemy::~Enemy() { delete PlayerModel_; }

void Enemy::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);
	worldTransform_.translation_ = position;
}

void Enemy::SetObstacleList(const std::vector<AABB>& obstacles) { obstacleList_.insert(obstacleList_.end(), obstacles.begin(), obstacles.end()); }

void Enemy::AddObstacle(const AABB& obstacle) { obstacleList_.push_back(obstacle); }

void Enemy::SetPosition(const Vector3& pos) {
	position = pos;
	worldTransform_.translation_ = position;
}

void Enemy::Update() {
	// 入力による移動
	//float moveSpeed = 0.0f;

	// 重力処理
	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	// プレイヤーのAABB作成（例：幅1.0, 高さ2.0, 奥行1.0）
	float halfW = 0.5f, halfH = 1.0f, halfD = 0.5f;
	AABB enemyAABB;
	enemyAABB.min = {position.x - halfW, position.y - halfH, position.z - halfD};
	enemyAABB.max = {position.x + halfW, position.y + halfH, position.z + halfD};

	// 反復的衝突解決（すり抜け防止のため、最大10回まで解決を試みる）
	const int maxIterations = 10;
	int iterations = 0;
	bool collisionOccurred = false;
	do {
		collisionOccurred = false;
		for (auto& obstacleAABB : obstacleList_) {
			if (IsCollisionAABB(enemyAABB, obstacleAABB)) {
				ResolveAABBCollision(enemyAABB, obstacleAABB, velocityY_, onGround_);
				collisionOccurred = true;
			}
		}
		iterations++;
	} while (collisionOccurred && iterations < maxIterations);

	// 衝突解決後のAABB中心をプレイヤー座標に反映
	position.x = (enemyAABB.min.x + enemyAABB.max.x) * 0.5f;
	position.y = (enemyAABB.min.y + enemyAABB.max.y) * 0.5f;
	position.z = (enemyAABB.min.z + enemyAABB.max.z) * 0.5f;

	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

}

void Enemy::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }

// AABBを取得するメソッドを定義
AABB Enemy::GetAABB() const {
	float halfW = 0.5f, halfH = 1.0f, halfD = 0.5f;
	AABB enemyAABB;
	enemyAABB.min = {position.x - halfW, position.y - halfH, position.z - halfD};
	enemyAABB.max = {position.x + halfW, position.y + halfH, position.z + halfD};
	return enemyAABB;
}