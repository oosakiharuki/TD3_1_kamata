#include "Player.h"

#include "imgui.h"
#include <iostream>
#include <algorithm>

#include <KamataEngine.h>


Player::Player() {}

Player::~Player() { delete PlayerModel_; }


void Player::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);
	worldTransform_.translation_ = position;
  
  worldTransform.Initialize();

	aabb = CreateAABB(worldTransform.translation_,size);

}


void Player::SetObstacleList(const std::vector<AABB>& obstacles) { obstacleList_.insert(obstacleList_.end(), obstacles.begin(), obstacles.end()); }


void Player::AddObstacle(const AABB& obstacle) { obstacleList_.push_back(obstacle); }

void Player::Update() {
	// 入力による移動
	float moveSpeed = 0.5f;
	if (Input::GetInstance()->PushKey(DIK_W)) {
		position.z += moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		position.z -= moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		position.x -= moveSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		position.x += moveSpeed;
	}

	// ジャンプ処理
	if (Input::GetInstance()->PushKey(DIK_SPACE) && onGround_) {
		velocityY_ = 0.3f;
		onGround_ = false;
	}


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
	
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
		!(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && onGround) {
		velocity.y = 0.0f;
		EnemyContral = false;
		controler = Controler::player;

	} else if (Input::GetInstance()->TriggerKey(DIK_K) && onGround) {
		velocity.y = 0.0f;
		EnemyContral = false;
		controler = Controler::player;
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


	// 重力処理
	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	// プレイヤーのAABB作成（例：幅1.0, 高さ2.0, 奥行1.0）
	float halfW = 0.5f, halfH = 1.0f, halfD = 0.5f;
	AABB playerAABB;
	playerAABB.min = {position.x - halfW, position.y - halfH, position.z - halfD};
	playerAABB.max = {position.x + halfW, position.y + halfH, position.z + halfD};

	// 反復的衝突解決（すり抜け防止のため、最大10回まで解決を試みる）
	const int maxIterations = 10;
	int iterations = 0;
	bool collisionOccurred = false;
	do {
		collisionOccurred = false;
		for (auto& obstacleAABB : obstacleList_) {
			if (IsCollisionAABB(playerAABB, obstacleAABB)) {
				ResolveAABBCollision(playerAABB, obstacleAABB, velocityY_, onGround_);
				collisionOccurred = true;
			}
		}
		iterations++;
	} while (collisionOccurred && iterations < maxIterations);

	// 衝突解決後のAABB中心をプレイヤー座標に反映
	position.x = (playerAABB.min.x + playerAABB.max.x) * 0.5f;
	position.y = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
	position.z = (playerAABB.min.z + playerAABB.max.z) * 0.5f;

	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

	cameraController_.Update(camera_, position);
}

void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }
