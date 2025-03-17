#pragma once
#include "KamataEngine.h"
#include "AABB.h"
#include "Key.h"
#include "Player.h"

using namespace KamataEngine;

class Door {
public:
	Door();
	~Door();

	// 初期化
	void Init(Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw();

	// プレイヤーとキーの参照を設定
	void SetPlayer(Player* player) { player_ = player; }
	void SetKey(Key* key) { key_ = key; }

	// ドアが開いたかどうか
	bool IsDoorOpened() const { return isDoorOpened_; }

	// ドアに触れたかどうか
	bool IsDoorTouched() const { return isDoorTouched_; }

	// AABBを取得
	AABB GetAABB() const;

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;
	Vector3 position_ = {1.5f, 0.0f, 48.592f}; // 指定された位置

	// 参照
	Player* player_ = nullptr;
	Key* key_ = nullptr;

	// ドア状態フラグ
	bool isDoorTouched_ = false; // ドアに触れたフラグ
	bool isDoorOpened_ = false;  // ドアが開いたフラグ


	// 開閉アニメーション用変数
	float openAngle_ = 0.0f;
	bool isAnimating_ = false;
};