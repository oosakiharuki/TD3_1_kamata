#pragma once
#include "KamataEngine.h"
#include "3d/Model.h"
#include "3d/Camera.h"
#include "3d/WorldTransform.h"
#include <assert.h>
#include <numbers>

using namespace KamataEngine;

class Skydome {

public:
	void Initialize(Model* model, Camera* viewProjection);

	void Update();

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;

	// モデル
	Model* model_ = nullptr;

	// モデルの移動速度や位置を格納する変数
	Vector3 velocity_; // 移動速度
};