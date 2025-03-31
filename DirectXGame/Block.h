#pragma once
#include "AABB.h"
#include "KamataEngine.h"
using namespace KamataEngine;


class Block {
public:
	~Block() { delete model_; }
	void Init(Camera* camera_);
	void Update();
	void Draw();

	bool IsActive() const { return isActive_; }         // アクティブ状態を取得
	void SetActive(bool active) { isActive_ = active; } // アクティブ状態を設定

	AABB GetAABB() const; // AABBの取得

	// 位置を設定するメソッドを追加
	void SetPosition(const Vector3& pos);

private:
	WorldTransform worldTransform;
	Camera* viewProjection_ = nullptr;
	Model* model_ = nullptr;
	bool isActive_ = true; // ブロックが有効かどうか
	uint32_t texturehandle_ = 0;
};