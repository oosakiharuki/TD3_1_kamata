#include "CannonEnemy.h"
#include "Bom.h"
#include "Player.h"
#include <algorithm>
#include <iostream>

CannonEnemy::CannonEnemy() {}

CannonEnemy::~CannonEnemy() {
	delete model_;
	for (Bom* bom : bullets_) {
		delete bom;
	}
}

void CannonEnemy::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	model_ = Model::CreateFromOBJ("cannon", true);
	worldTransform_.translation_ = position;
}

void CannonEnemy::SetObstacleList(const std::vector<AABB>& obstacles) { obstacleList_.insert(obstacleList_.end(), obstacles.begin(), obstacles.end()); }

void CannonEnemy::AddObstacle(const AABB& obstacle) { obstacleList_.push_back(obstacle); }

void CannonEnemy::Update() {

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
				} else {
					onGround_ = false;
				}
			}
			iterations++;
		} while (collisionOccurred && iterations < maxIterations);

		// 衝突解決後のAABB中心をプレイヤー座標に反映
		position.x = (enemyAABB.min.x + enemyAABB.max.x) * 0.5f;
		position.y = (enemyAABB.min.y + enemyAABB.max.y) * 0.5f;
		position.z = (enemyAABB.min.z + enemyAABB.max.z) * 0.5f;

		/// 敵の移動、攻撃　ここから

		if (!isStan) {

			AABB playerAABB = player_->GetAABB();

			Vector3 closestPoint{
			    std::clamp(position.x, playerAABB.min.x, playerAABB.max.x), std::clamp(position.y, playerAABB.min.y, playerAABB.max.y), std::clamp(position.z, playerAABB.min.z, playerAABB.max.z)
			};

			float distance = Length(closestPoint - position);

			// 円にプレイヤーが入ってきたとき弾を発射する
			if (distance <= radius) {
				Fire();
			}

			// timer += deltaTime;

			// if (timer > corveTime && collisionOccurred) {
			//	if (Normal) {
			//		Normal = false;
			//	} else {
			//		Normal = true;
			//	}
			//	timer = 0.0f;
			// }
			// if (Normal) {
			//	position.z += moveSpeed_;
			// } else {
			//	position.z -= moveSpeed_;
			// }
		}

		/// ここまで

		// プレイヤーの方向を向く処理
		if (player_) {
			Vector3 playerPosition = player_->GetWorldPosition();
			Vector3 direction = playerPosition - position;

			// atan2を使ってY軸の回転角度を計算
			worldTransform_.rotation_.y = atan2(direction.x, -direction.z);
		}

		worldTransform_.translation_ = position;
	}
#ifdef _DEBUG
	ImGui::Begin("enemy");
	ImGui::DragFloat3("translate", &position.x);
	ImGui::End();
#endif
	for (Bom* bom : bullets_) {
		bom->Update();
	}

	bullets_.remove_if([](Bom* bom) {
		if (bom->IsDaed()) {
			delete bom;
			return true;
		}
		return false;
	});

	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void CannonEnemy::Draw() {

	model_->Draw(worldTransform_, *camera_);

	for (Bom* bom : bullets_) {
		bom->Draw(camera_);
	}
}

void CannonEnemy::Fire() {
	const float deltaTimer = 1.0f / 60.0f;
	fireTimer -= deltaTimer;

	if (fireTimer <= 0.0f) {
		const float kSpeed = 0.5f;

		// プレイヤーの位置を取得
		Vector3 playerPosition = player_->GetWorldPosition();
		Vector3 enemyPosition = worldTransform_.translation_;

		// プレイヤーへの方向ベクトルを計算（ワールド座標系）
		Vector3 direction = playerPosition - enemyPosition;
		Vector3 normalize = Normalize(direction); // 正規化

		// 速度ベクトルをプレイヤーの方向に設定
		Vector3 velocity = normalize * kSpeed;

		// 新しい弾を生成
		Bom* newBullet = new Bom();
		newBullet->Init(enemyPosition, velocity);

		bullets_.push_back(newBullet);

		// 次の発射までのクールダウン
		fireTimer = 2.0f;
	}
}

void CannonEnemy::PlayerFire() {
	const float kSpeed = 0.5f;

	Vector3 velocity(0, 0, kSpeed);

	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	Bom* newBullet = new Bom();
	newBullet->Init(player_->GetWorldPosition(), velocity);

	bullets_.push_back(newBullet);
}

// AABBを取得するメソッドを定義
AABB CannonEnemy::GetAABB() {
	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f;
	AABB enemyAABB;
	enemyAABB.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	enemyAABB.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};
	return enemyAABB;
}

void CannonEnemy::ContralPlayer() {
	isPlayer = true;
	worldTransform_.translation_ = {0, -2, 0};
	worldTransform_.rotation_ = { 0, 0, 0 };
	if (player_) {
		player_->SetState(Player::State::Bomb);
	}
}

void CannonEnemy::ReMove(const Vector3& position_) {
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