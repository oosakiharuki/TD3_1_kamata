#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "CameraController.h"
#include "Ground.h" // 追加：Groundクラスのヘッダをインクルード
#include "math/Vector3.h"

using namespace KamataEngine;

class Player {
public:
	Player();
	~Player();
	void Init(Camera* camera);
	void Update();
	void Draw();
	// Groundオブジェクトを設定するためのメソッド
	void SetGround(Ground* ground);

private:
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
	Model* PlayerModel_ = nullptr;
	// プレイヤーの座標
	Vector3 position = {0, 0, 0};
	// ジャンプ/重力処理用の変数
	bool onGround_ = true;
	float velocityY_ = 0.0f;
	// カメラ追従用のクラス
	CameraController cameraController_;
	// 追加：地面オブジェクトへのポインタ
	Ground* ground_ = nullptr;
};
