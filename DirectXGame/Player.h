#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "CameraController.h"
#include "math/Vector3.h"

using namespace KamataEngine;

class Player {
public:
	Player();
	~Player();
	void Init(Camera* camera);
	void Update();
	void Draw();

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;

	// プレイヤーの座標
	Vector3 position = {0, 0, 0};

	// 簡易的なジャンプ/重力処理用の変数
	bool onGround_ = true;   // 地面に着地しているか
	float velocityY_ = 0.0f; // 上下方向の速度

	// カメラの追従処理を担当するクラス
	CameraController cameraController_;
};
