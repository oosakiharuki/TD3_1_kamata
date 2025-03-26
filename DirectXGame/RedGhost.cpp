#include "RedGhost.h"
#include "AABB.h"
#include "Collision.h"
#include <KamataEngine.h>
#include <algorithm>
#include <iostream>

RedGhost::RedGhost() {}

RedGhost::~RedGhost() { delete PlayerModel_; }

void RedGhost::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("RedGhost", true);
	worldTransform_.translation_ = position;
}

void RedGhost::SetObstacleList(const std::vector<AABB>& obstacles) { obstacleList_.insert(obstacleList_.end(), obstacles.begin(), obstacles.end()); }

void RedGhost::AddObstacle(const AABB& obstacle) { obstacleList_.push_back(obstacle); }

void RedGhost::SetPosition(const Vector3& pos) {
	position = pos;
	worldTransform_.translation_ = position;
}

void RedGhost::SetTarget(Player* target) {
	player_ = target; // プレイヤーをターゲットとして設定
}

void RedGhost::Update() {
	// 入力による移動
	// float moveSpeed = 0.0f;

	// const float moveSpeed_ = 0.02f;
	const float deltaTime = 1.0f / 60.0f;

	if (isStan) {
		timerS += deltaTime;
		if (timerS > stanTime) {
			isStan = false;
			timerS = 0.0f;
		}
	}

	if (!isPlayer) {

		// 重力処理
		float gravity = 0.01f;
		velocityY_ -= gravity;
		position.y += velocityY_;

		// プレイヤーのAABB作成（例：幅1.0, 高さ2.0, 奥行1.0）
		float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f;
		AABB RedGhostAABB;
		RedGhostAABB.min = { position.x - halfW, position.y - halfH, position.z - halfD };
		RedGhostAABB.max = { position.x + halfW, position.y + halfH, position.z + halfD };

		// 反復的衝突解決（すり抜け防止のため、最大10回まで解決を試みる）
		const int maxIterations = 10;
		int iterations = 0;
		bool collisionOccurred = false;
		do {
			collisionOccurred = false;
			for (auto& obstacleAABB : obstacleList_) {
				if (IsCollisionAABB(RedGhostAABB, obstacleAABB)) {
					ResolveAABBCollision(RedGhostAABB, obstacleAABB, velocityY_, onGround_);
					collisionOccurred = true;
				}
				else {
					onGround_ = false;
				}
			}
			iterations++;
		} while (collisionOccurred && iterations < maxIterations);

		// 衝突解決後のAABB中心をプレイヤー座標に反映
		position.x = (RedGhostAABB.min.x + RedGhostAABB.max.x) * 0.5f;
		position.y = (RedGhostAABB.min.y + RedGhostAABB.max.y) * 0.5f;
		position.z = (RedGhostAABB.min.z + RedGhostAABB.max.z) * 0.5f;

		Vector3 move = worldTransform_.translation_;

		if (IsPlayerInChaseRadius()) {
			// プレイヤーへのベクトルを計算
			Vector3 playerWorldPosition = player_->GetWorldPosition();
			Vector3 RedGhostWorldPosition = GetWorldPosition();
			Vector3 toPlayer = Normalize(playerWorldPosition - RedGhostWorldPosition);

			Vector3 direction = playerWorldPosition - RedGhostWorldPosition;
			direction.y = 0; // Y軸回転のみなので高さは無視
			direction = Normalize(direction);

			// Y軸の回転角度を計算
			worldTransform_.rotation_.y = atan2(direction.x, -direction.z);

			// 一定速度でプレイヤーに向かうベクトルで設定
			velocity = toPlayer * kChaseSpeed;

			// Playerとの衝突判定
			if (CheckCollisionWithPlayer()) {
				// 衝突時の処理（移動を停止）
				velocity.x = 0;
				velocity.z = 0;
			}
			else {
				// 速度をそのまま適用
				worldTransform_.translation_.x += velocity.x;
				worldTransform_.translation_.z += velocity.z;
			}
		}

		worldTransform_.translation_.y = position.y;
	}
#ifdef _DEBUG
	ImGui::Begin("RedGhost");
	ImGui::DragFloat3("translate", &position.x);
	ImGui::End();
#endif
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void RedGhost::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }

// AABBを取得するメソッドを定義
AABB RedGhost::GetAABB() const {
	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f;
	AABB RedGhostAABB;
	RedGhostAABB.min = { worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD };
	RedGhostAABB.max = { worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD };
	return RedGhostAABB;
}

void RedGhost::ContralPlayer() {
	isPlayer = true;
	worldTransform_.translation_ = { 0, -2, 0 };
	worldTransform_.rotation_ = { 0, 3, 0 };
	if (player_) {
		player_->SetState(Player::State::Ghost);
	}
}

void RedGhost::ReMove(const Vector3& position_) {
	if (isPlayer) {
		position.x = position_.x;
		position.y = position_.y - 2;
		position.z = position_.z;
		timerS = 0.0f;
		isStan = true;
		isPlayer = false;
		worldTransform_.parent_ = nullptr;
		if (player_) {
			player_->SetState(Player::State::Normal);
		}
	}
}

Vector3 RedGhost::GetWorldPosition() {

	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

bool RedGhost::CheckCollisionWithPlayer() {
	AABB playerAABB = player_->GetAABB();
	AABB RedGhostAABB = GetAABB();
	return IsCollisionAABB(playerAABB, RedGhostAABB);
}

void RedGhost::SetChaseRadius(float radius_) { chaseRadius_ = radius_; }

// プレイヤーが追尾範囲内にいるかどうかを判定
bool RedGhost::IsPlayerInChaseRadius() {
	Vector3 playerWorldPosition = player_->GetWorldPosition();
	Vector3 RedGhostWorldPosition = GetWorldPosition();
	float distance = Length(playerWorldPosition - RedGhostWorldPosition);
	return distance <= chaseRadius_;
}
