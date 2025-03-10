#include "Player.h"
#include <KamataEngine.h>

Player::Player() {}

Player::~Player() { delete PlayerModel_; }

void Player::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);
	worldTransform_.translation_ = position;
}

void Player::SetObstacleList(const std::vector<AABB>& obstacles) { obstacleList_.insert(obstacleList_.end(), obstacles.begin(), obstacles.end()); }

void Player::AddObstacle(const AABB& obstacle) { obstacleList_.push_back(obstacle); }

void Player::Update() {
	// 入力による移動
	float moveSpeed = 0.5f;
	if (Input::GetInstance()->PushKey(DIK_W)) {
		position.z += moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		position.z -= moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		position.x -= moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		position.x += moveSpeed;
	}

	// ジャンプ処理
	if (Input::GetInstance()->PushKey(DIK_SPACE) && onGround_) {
		velocityY_ = 0.3f;
		onGround_ = false;
	}

	// 重力処理
	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	// プレイヤーのAABB作成（例：幅1.0, 高さ2.0, 奥行1.0）
	float halfW = 0.5f, halfH = 1.0f, halfD = 0.5f;
	AABB playerAABB;
	playerAABB.min = {position.x - halfW, position.y - halfH, position.z - halfD};
	playerAABB.max = {position.x + halfW, position.y + halfH, position.z + halfD};

	// 反復的衝突解決（すり抜け防止のため、最大10回まで解決を試みる）
	const int maxIterations = 10;
	int iterations = 0;
	bool collisionOccurred = false;
	do {
		collisionOccurred = false;
		for (auto& obstacleAABB : obstacleList_) {
			if (IsCollisionAABB(playerAABB, obstacleAABB)) {
				ResolveAABBCollision(playerAABB, obstacleAABB, velocityY_, onGround_);
				collisionOccurred = true;
			}
		}
		iterations++;
	} while (collisionOccurred && iterations < maxIterations);

	// 衝突解決後のAABB中心をプレイヤー座標に反映
	position.x = (playerAABB.min.x + playerAABB.max.x) * 0.5f;
	position.y = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
	position.z = (playerAABB.min.z + playerAABB.max.z) * 0.5f;

    // Enemyとの衝突判定
	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		AABB enemyAABB = (*it)->GetAABB();
		if (IsCollisionAABB(playerAABB, enemyAABB)) {
			// 衝突時の処理（例：リストから削除）
			it = enemyList_.erase(it);
		} else {
			++it;
		}
	}

	/*/
	// Enemyとの衝突判定
	for (const auto& enemy : enemyList_) {
		AABB enemyAABB = enemy->GetAABB();
		if (IsCollisionAABB(playerAABB, enemyAABB)) {
			// 衝突時の処理をここに記述
		}
	}
	/*/

	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

	cameraController_.Update(camera_, position);
}


void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }

void Player::SetEnemyList(const std::vector<Enemy*>& enemies) { enemyList_ = enemies; }