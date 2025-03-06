#include "Player.h"
#include "imgui.h"
#include <iostream>
#include <algorithm>

void Player::Init(Model* model, Camera* viewProjection) { 

	model_ = model;
	viewProjection_ = viewProjection;

	worldTransform.Initialize();

	aabb = CreateAABB(worldTransform.translation_,size);
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
		worldTransform.translation_.x += x * speed;
		worldTransform.translation_.z += z * speed;

	} else {

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
	}


	switch (controler) {
	case Controler::player:

		if (onEnemy || worldTransform.translation_.y <= 0.0f) {
			onGround = true;
		} else {
			onGround = false;
		}

		if (!onGround) {
			// のりうつるときの処理
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !EnemyContral) {
				velocity.y -= 1.2f;
				isTransfar = true;
			} else if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !EnemyContral) {
				velocity.y -= 1.2f;
				isTransfar = true;
			}
		} else {
			velocity.y = 0.0f;
			isTransfar = false;
		}
		break;
	case Controler::enemyTransfar:

		if (onEnemy || worldTransform.translation_.y <= 2.0f) {
			onGround = true;
		} else {
			onGround = false;
		}


		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
			!(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && onGround) {
			EnemyContral = false;
			controler = Controler::player;

		} else if (Input::GetInstance()->TriggerKey(DIK_K) && onGround) {
			velocity.y = 0.0f;
			EnemyContral = false;
			controler = Controler::player;
		}
 
		break;
	default:
		break;
	}

	
	velocity.y -= 0.1f;
	

	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
		!(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && onGround) {
		onGround = false;
		velocity.y = 1.2f;
	} else if (Input::GetInstance()->TriggerKey(DIK_SPACE) && onGround) {
		onGround = false;
		velocity.y = 1.2f;
	}




	worldTransform.translation_.y += velocity.y;
	worldTransform.translation_.y = std::clamp(worldTransform.translation_.y, yuka, 1000.0f);



	aabb = CreateAABB(worldTransform.translation_,size);

	ImGui::Begin("test");
	ImGui::DragFloat3("translate", &worldTransform.translation_.x);
	ImGui::DragFloat3("aabbMax", &aabb.max.x);
	ImGui::DragFloat3("aabbMin", &aabb.min.x);
	ImGui::End();


	worldTransform.UpdateMatrix();
}

void Player::Draw() { 
	model_->Draw(worldTransform,*viewProjection_);
}


