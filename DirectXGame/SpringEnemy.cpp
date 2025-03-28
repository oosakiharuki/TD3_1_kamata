#include "SpringEnemy.h"
#include "Player.h"
#include <KamataEngine.h>

SpringEnemy::SpringEnemy() {}

SpringEnemy::~SpringEnemy() { delete model_; }

void SpringEnemy::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube"モデルを読み込み、後で専用のばねモデルに置き換え可能
	model_ = Model::CreateFromOBJ("spring", true);
	worldTransform_.translation_ = position;

	// ばね敵用の特徴的なスケール設定（高さがあり、幅が狭い）
	worldTransform_.scale_ = {0.7f, 1.2f, 0.7f};
	originalScaleY = worldTransform_.scale_.y;

	// 重力と落下を無効化
	velocityY_ = 0.0f;
	onGround_ = true;
}

void SpringEnemy::SetObstacleList(const std::vector<AABB>& obstacles) { obstacleList_.insert(obstacleList_.end(), obstacles.begin(), obstacles.end()); }

void SpringEnemy::AddObstacle(const AABB& obstacle) { obstacleList_.push_back(obstacle); }

void SpringEnemy::SetPosition(const Vector3& pos) {
	position = pos;
	worldTransform_.translation_ = position;
	// 位置を設定したら必ず行列を更新する
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void SpringEnemy::Update() {
	const float deltaTime = 1.0f / 60.0f;

	// スタン状態管理
	if (isStan) {
		timerS += deltaTime;
		if (timerS > stanTime) {
			isStan = false;
			timerS = 0.0f;
		}
	}

	if (!isPlayer) {
		// 重力処理を無効化し、位置を固定
		velocityY_ = 0.0f;

		// コリジョン用AABB
		float halfW = 0.7f, halfH = 1.2f, halfD = 0.7f;
		AABB enemyAABB;
		enemyAABB.min = {position.x - halfW, position.y - halfH, position.z - halfD};
		enemyAABB.max = {position.x + halfW, position.y + halfH, position.z + halfD};

		// 地面との衝突のみチェック（横方向の衝突は無視）
		for (auto& obstacleAABB : obstacleList_) {
			if (IsCollisionAABB(enemyAABB, obstacleAABB)) {
				// Y座標のみ調整
				if (enemyAABB.min.y < obstacleAABB.max.y && enemyAABB.max.y > obstacleAABB.max.y) {
					position.y = obstacleAABB.max.y + halfH;
					onGround_ = true;
				}
			}
		}

		// 圧縮アニメーション処理
		if (isCompressed) {
			compressionTimer += deltaTime;

			// 視覚的フィードバック：Y軸スケールを変更して圧縮を表現
			float compressionProgress = compressionTimer / 0.3f; // アニメーション時間は0.3秒
			if (compressionProgress < 0.5f) {
				// 前半：圧縮
				worldTransform_.scale_.y = originalScaleY * (1.0f - 0.5f * (compressionProgress / 0.5f));
			} else {
				// 後半：元に戻る
				worldTransform_.scale_.y = originalScaleY * (0.5f + 0.5f * ((compressionProgress - 0.5f) / 0.5f));
			}

			// アニメーション終了処理
			if (compressionTimer >= 0.3f) {
				isCompressed = false;
				compressionTimer = 0.0f;
				worldTransform_.scale_.y = originalScaleY; // スケールをリセット
			}
		}

		worldTransform_.translation_ = position;
	}
#ifdef _DEBUG
	// デバッグUI
	ImGui::Begin("SpringEnemy");
	ImGui::DragFloat3("Position", &position.x);
	ImGui::DragFloat("Jump Boost", &jumpBoostFactor, 0.1f, 1.0f, 5.0f);
	ImGui::Text("Compressed: %s", isCompressed ? "Yes" : "No");
	ImGui::End();
#endif

	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void SpringEnemy::Draw() { model_->Draw(worldTransform_, *camera_); }

AABB SpringEnemy::GetAABB() const {
	// スケールに基づいたAABBの調整
	float halfW = 0.7f, halfH = 1.2f, halfD = 0.7f;
	AABB enemyAABB;
	enemyAABB.min = {worldTransform_.translation_.x - halfW, worldTransform_.translation_.y - halfH, worldTransform_.translation_.z - halfD};
	enemyAABB.max = {worldTransform_.translation_.x + halfW, worldTransform_.translation_.y + halfH, worldTransform_.translation_.z + halfD};
	return enemyAABB;
}

void SpringEnemy::Compress() {
	isCompressed = true;
	compressionTimer = 0.0f;
}

void SpringEnemy::ContralPlayer() {
	isPlayer = true;
	worldTransform_.translation_ = {0, -2, 0};
}

void SpringEnemy::ReMove(const Vector3& position_) {
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

void SpringEnemy::ClearObstacleList() {
	obstacleList_.clear(); // ✅ クリア
}