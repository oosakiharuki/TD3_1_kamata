#pragma once
#include"KamataEngine.h"
#include "AABB.h"
#include "Player.h"

using namespace KamataEngine;

class Key {
public:
	Key();
	~Key();

	// 初期化
	void Init(Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw();

	// プレイヤーの参照を設定
	void SetPlayer(Player* player) { player_ = player; }

	// キーが取得されたかどうか
	bool IsKeyObtained() const { return isObtained_; }

	// AABBを取得
	AABB GetAABB() const;

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;
	Vector3 position_ = {3.499f, 9.510f, -47.592f}; // 指定された位置

	// プレイヤー参照
	Player* player_ = nullptr;

	// 鍵取得フラグ
	bool isObtained_ = false;

	// 回転アニメーション用
	float rotationY_ = 0.0f;
};