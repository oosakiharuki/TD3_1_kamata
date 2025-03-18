#pragma once
#include "AABB.h"
#include "Collision.h"
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

class Player;      // 前方宣言
class SpringEnemy; // 前方宣言

class SpringEnemy {
public:
	SpringEnemy();
	~SpringEnemy();

	void Init(Camera* camera);
	void Update();
	void Draw();

	// 障害物処理
	void SetObstacleList(const std::vector<AABB>& obstacles);
	void AddObstacle(const AABB& obstacle);

	// 位置設定
	void SetPosition(const Vector3& pos);

	// 衝突判定用AABB
	AABB GetAABB() const;

	// プレイヤー参照
	void SetPlayer(Player* player) { player_ = player; }

	// スプリング専用メソッド
	float GetJumpBoost() const { return jumpBoostFactor; }
	void Compress();

	// プレイヤー操作メソッド（Enemyと同様）
	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }
	void ContralPlayer();
	void ReMove(const Vector3& position_);
	bool GetPlayerCtrl() { return isPlayer; }

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;
	Vector3 position = {0, 0, -20};
	bool onGround_ = true;
	float velocityY_ = 0.0f;

	// 障害物リスト
	std::vector<AABB> obstacleList_;

	// プレイヤー参照
	Player* player_ = nullptr;

	// スプリングのプロパティ
	float jumpBoostFactor = 1.2f;  // ジャンプブースト
	bool isCompressed = false;     // バネの見た目状態
	float compressionTimer = 0.0f; // アニメーション用タイマー
	float originalScaleY = 1.0f;   // 元のスケール（復元用）

	// プレイヤー操作（Enemyと同様）
	bool isPlayer = false;
	bool isStan = false;
	float timerS = 0.0f;
	const float stanTime = 3.0f;

	// デバッグ用
	XINPUT_STATE state, preState;
};
