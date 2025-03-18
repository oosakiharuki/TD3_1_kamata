#pragma once
#include "KamataEngine.h"
#include "AABB.h"
using namespace KamataEngine;

class Block {
public:
	void Init(Model* model, Camera* viewProjection, uint32_t texturehandle);
    void Update();
    void Draw();

    bool IsActive() const { return isActive_; } // アクティブ状態を取得
    void SetActive(bool active) { isActive_ = active; } // アクティブ状態を設定

    AABB GetAABB() const; // AABBの取得

private:
    WorldTransform worldTransform;
    Camera* viewProjection_ = nullptr;
    Model* model_ = nullptr;
    bool isActive_ = true; // ブロックが有効かどうか
	uint32_t texturehandle_ = 0;
};