#pragma once
#include "3d/WorldTransform.h"
#include "math/Vector3.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "input/input.h"

using namespace KamataEngine;

enum class PlayerState {
	Normal,
	Bom,
	Ghost
};

class Player {
public:
	void Init(Model* model,Camera* viewProjection);
	void Update();
	void Draw();

	void SetState(PlayerState newState);  // 状態変更メソッド
	PlayerState GetState() const { return state_; }

private:
	WorldTransform worldTransform;
	Camera* viewProjection_ = nullptr;
	Model* model_ = nullptr;

	Vector3 position;
	Vector3 velocity;
	bool IsJump = false;

	PlayerState state_ = PlayerState::Normal; // 初期状態はNormal

	XINPUT_STATE state, preState;
	const float speed = 0.2f;
};