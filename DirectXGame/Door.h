#pragma once
#include "AABB.h"
#include "KamataEngine.h"
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

	// 位置を設定（CSVから読み込んだ位置に合わせるため）
	void SetPosition(const Vector3& position) {
		position_ = position;
		worldTransform_.translation_ = position;
	}

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;
	Vector3 position_ = {1.5f, 0.0f, 48.592f}; // デフォルト位置（CSVから上書き可能）

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