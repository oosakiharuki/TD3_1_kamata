#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "math/Vector3.h"

using namespace KamataEngine;
namespace KamataEngine { class Input;}

class Player {
public:

	void Init(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& pos);
	void Update();
	void Draw();

	KamataEngine::Vector3 GetWorldPosition(); 
	void SetParent(const WorldTransform* parent);

private:
	WorldTransform worldTransform_;
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;

	KamataEngine::Model* model_ = nullptr;
	Model* PlayerModel_ = nullptr;


	// プレイヤーの座標
	Vector3 position = {0, 0, 0};
	// 簡易的なジャンプ/重力処理用の変数
	bool onGround_ = true;   // 地面に着地しているか
	float velocityY_ = 0.0f; // 上下方向の速度

};
