#include "Player.h"

void Player::Init(Model* model, Camera* viewProjection) { 

	model_ = model;
	viewProjection_ = viewProjection;

	worldTransform.Initialize();
}

void Player::Update() { 
	float x = 0, z = 0;

	Input::GetInstance()->GetJoystickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0,preState);

	if (Input::GetInstance()->GetJoystickState(0, state)) {
		// 左スティックの入力
		x = static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f; // -1.0f～1.0f
		z = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f; // -1.0f～1.0f

		// デッドゾーン処理
		const float deadZone = 0.2f; // スティックの感度調整
		if (abs(x) < deadZone) {
			x = 0.0f;
		}
		if (abs(z) < deadZone) {
			z = 0.0f;
		}
	}

	if (IsJump) {
		//のりうつるときの処理
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && 
			!(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			velocity.y -= 1.2f;
		} else {
			velocity.y -= 0.1f;
		}
	} else {
		velocity.y = 0.0f;
	}

	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
		!(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !IsJump) {
		IsJump = true;
		velocity.y = 1.5f;
	} 


	if (worldTransform.translation_.y < 0.0f) {
		worldTransform.translation_.y = 0.0f;
		IsJump = false;
	}

	worldTransform.translation_.y += velocity.y;


	worldTransform.translation_.x += x * speed;
	worldTransform.translation_.z += z * speed;

	if (Input::GetInstance()->PushKey(DIK_A)) {
		worldTransform.translation_.x -= 0.1f; 
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		worldTransform.translation_.x += 0.1f;
	}

	if (Input::GetInstance()->PushKey(DIK_S)) {
		worldTransform.translation_.z -= 0.1f;
	}
	if (Input::GetInstance()->PushKey(DIK_W)) {
		worldTransform.translation_.z += 0.1f;
	}


	worldTransform.UpdateMatrix();
}

void Player::Draw() { 
	model_->Draw(worldTransform,*viewProjection_);
}

// Player.cpp
void Player::SetState(PlayerState newState) {
	state_ = newState;
}