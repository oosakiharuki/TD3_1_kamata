#pragma once
#include "AABB.h"
#include "KamataEngine.h"
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

	// 位置を設定（CSVから読み込んだ位置に合わせるため）
	void SetPosition(const Vector3& position) {
		position_ = position;
		worldTransform_.translation_ = position;
	}

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* model_ = nullptr;
	Vector3 position_ = {3.499f, 9.510f, -47.592f}; // デフォルト位置（CSVから上書き可能）

	// プレイヤー参照
	Player* player_ = nullptr;

	// 鍵取得フラグ
	bool isObtained_ = false;

	// 回転アニメーション用
	float rotationY_ = 0.0f;
};