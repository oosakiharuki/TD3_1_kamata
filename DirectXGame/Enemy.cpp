#include "Enemy.h"
#include "AABB.h"
#include "Collision.h"
#include <KamataEngine.h>

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

void Enemy::SetTarget(Player* target) {
	player_ = target; // プレイヤーをターゲットとして設定
}

void Enemy::Update() {
	// 入力による移動
	// float moveSpeed = 0.0f;

	const float moveSpeed_ = 0.02f;
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
			timer += deltaTime;

			if (timer > corveTime && collisionOccurred) {
				if (Normal) {
					Normal = false;
				} else {
					Normal = true;
				}
				timer = 0.0f;
			}
			if (Normal) {
				position.z += moveSpeed_;
			} else {
				position.z -= moveSpeed_;
			}
		}

		/// ここまで

		Vector3 move = worldTransform_.translation_;

		// 初期速度
		const float kBulletSpeed = 0.005f;

		// プレイヤーへのベクトルを計算
		Vector3 playerWorldPosition = player_->GetWorldPosition();
		Vector3 enemyWorldPosition = GetWorldPosition();
		Vector3 toPlayer = Normalize(playerWorldPosition - enemyWorldPosition);

		// 初期速度はプレイヤーに向かうベクトルで設定
		velocity = toPlayer * kBulletSpeed;

		// 現在の進行方向を少しずつプレイヤー方向に補正
		float adjustmentFactor = 0.045f; // 補正の割合。値を調整することで追尾の滑らかさを変更
		velocity_ = Normalize(velocity_ + toPlayer * adjustmentFactor);

		Enemy* newenemy = new Enemy();
		// ターゲット（プレイヤー）を追尾するように設定
		newenemy->SetTarget(player_);

		float movementSpeed = 0.5f; // 移動速度の調整
		worldTransform_.translation_.x += velocity_.x * movementSpeed;
		worldTransform_.translation_.y += velocity_.y * movementSpeed;
		worldTransform_.translation_.z += velocity_.z * movementSpeed;

		worldTransform_.translation_ = position;
	}

	ImGui::Begin("enemy");
	ImGui::DragFloat3("translate", &position.x);
	ImGui::End();

	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }

// AABBを取得するメソッドを定義
AABB Enemy::GetAABB() const {
	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f;
	AABB enemyAABB;
	enemyAABB.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	enemyAABB.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};
	return enemyAABB;
}

void Enemy::ContralPlayer() {
	isPlayer = true;
	worldTransform_.translation_ = {0, -2, 0};
}

void Enemy::ReMove(const Vector3& position_) {
	if (isPlayer) {
		position.x = position_.x;
		position.y = position_.y - 2;
		position.z = position_.z;
		timerS = 0.0f;
		isStan = true;
		isPlayer = false;
		worldTransform_.parent_ = nullptr;
	}
}

Vector3 Enemy::GetWorldPosition() {

	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}