#include "Player.h"

//#include "imgui.h"
//#include <iostream>
//#include <algorithm>
//
/////================
//#include <KamataEngine.h>

#include "imgui.h"
#include <KamataEngine.h>
#include <algorithm>
#include <iostream>


Player::Player() {}

Player::~Player() { delete PlayerModel_; }

void Player::Init(Camera* camera, uint32_t texture) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);
	worldTransform_.translation_ = position;
	textureHandle = texture;
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
	
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		currentState = State::Normal;
	}
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		currentState = State::Bomb;
	}
	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		currentState = State::Ghost;
	}

	float x = 0, z = 0;
	float xCamera = 0, zCamera = 0;

	float angle = 0;


	const float deadZone = 0.2f; // スティックの感度調整

	Input::GetInstance()->GetJoystickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

	if (Input::GetInstance()->GetJoystickState(0, state)) {

		// 右スティックの入力
		xCamera = static_cast<float>(state.Gamepad.sThumbRX) / 32768.0f; // -1.0f～1.0f
		//zCamera = static_cast<float>(state.Gamepad.sThumbRY) / 32768.0f; // -1.0f～1.0f
		                                                   
		// デッドゾーン処理      
		if (abs(xCamera) < deadZone) {
			xCamera = 0.0f;
		}
		if (abs(zCamera) < deadZone) {
			zCamera = 0.0f;
		}


		// 回転
		const float rotate = 0.7f;

		cameraYaw += xCamera;	
		//cameraPitch += zCamera;

		// 左スティックの入力
		x = static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f; // -1.0f～1.0f
		z = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f; // -1.0f～1.0f

		// デッドゾーン処理
		if (abs(x) < deadZone) {
			x = 0.0f;
		if (abs(z) < deadZone)
			z = 0.0f;
		}
		// 回転
		//const float rotate = 0.7f;
		bool isMoving = false;

		Vector3 RotateMove = {x , 0.0f, z};
		if (Length(RotateMove) > rotate) {
			isMoving = true;
		}

		Vector3 move = {x, 0.0f, z};
		
		if (isMoving) {
			move = Normalize(move) * speed;
	
			move = TransformNormal(move, worldTransform_.matWorld_);
			angle = std::atan2(RotateMove.x, RotateMove.z);
			//worldTransform_.rotation_.y = -angle;
			

			position.x += move.x;
			position.z += move.z;
			
		}

	}

  
	// QとEキーの入力処理
	if (Input::GetInstance()->PushKey(DIK_Q)) {
		cameraYaw -= 1.0f; // Qキーで左回転
	}
	if (Input::GetInstance()->PushKey(DIK_E)) {
		cameraYaw += 1.0f; // Eキーで右回転
	}
	
	//cameraController_.SetPitch(cameraPitch);
	cameraController_.SetYaw(cameraYaw);
	worldTransform_.rotation_.y = -(cameraYaw * (3.14159265f / 180.0f));

	if (!onGround_) {
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !EnemyContral && !isTransfar) {
			velocityY_ -= 1.2f;
			isTransfar = true;
		} else if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !EnemyContral && !isTransfar) {
			velocityY_ -= 1.2f;
			isTransfar = true;
		}	    
	} else {
		isTransfar = false;
	}

	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && onGround_) {
		velocityY_ = 0.3f;
		onGround_ = false;
	} else if (Input::GetInstance()->TriggerKey(DIK_SPACE) && onGround_) {
		velocityY_ = 0.3f;
		onGround_ = false;
	}


	//position.x += x * speed;
	//position.z += z * speed;

	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && onGround_ && EnemyContral) {
		velocityY_ = 0.0f;
		EnemyContral = false;
		onEnemy = true;
	} else if (Input::GetInstance()->TriggerKey(DIK_K) && onGround_ && EnemyContral) {
		velocityY_ = 0.0f;
		EnemyContral = false;
		onEnemy = true;
	}

	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f;
	playerAABB.min = {position.x - halfW, position.y - halfH, position.z - halfD};
	playerAABB.max = {position.x + halfW, position.y + halfH, position.z + halfD};

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

	AABB cannonAABB = cannonEnemy->GetAABB();

	if (IsCollisionAABB(playerAABB, cannonAABB) && !EnemyContral) {
		ResolveAABBCollision(playerAABB, cannonAABB, velocityY_, onGround_);
		if (isTransfar && (playerAABB.min.y >= cannonAABB.max.y)) {
			cannonEnemy->ContralPlayer();
			EnemyContral = true;
			collisionEnemy = true;
		}
	}

	if (EnemyContral && cannonEnemy->GetPlayerCtrl()) {
		cannonEnemy->SetParent(&worldTransform_);
    
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) && 
			!(preState.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {
			cannonEnemy->PlayerFire(); // カメラ向きに発射される
		} 
		else if (Input::GetInstance()->TriggerKey(DIK_J)) {
			cannonEnemy->PlayerFire();//カメラ向きに発射される
		}
	} else {
		cannonEnemy->ReMove(worldTransform_.translation_);
	}

	CheckCollision();

	// 衝突解決：プレイヤーがドアにめり込まないようにする
	if (IsCollisionAABB(playerAABB, doorAABB) && !isOpenDoor) {
		ResolveAABBCollision(playerAABB, doorAABB, velocityY_, onGround_);
	}

	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		enemyAABB = (*it)->GetAABB();
		if (IsCollisionAABB(playerAABB, enemyAABB) && !EnemyContral) {
			ResolveAABBCollision(playerAABB, enemyAABB, velocityY_, onGround_);
			if (isTransfar && (playerAABB.min.y >= enemyAABB.max.y)) {
				(*it)->ContralPlayer();
				EnemyContral = true;
				collisionEnemy = true;
			}
		}

		if (EnemyContral && (*it)->GetPlayerCtrl()) {
			(*it)->SetParent(&worldTransform_);
		} else {
			(*it)->ReMove(worldTransform_.translation_);
		}
		++it;
	}

	position.x = (playerAABB.min.x + playerAABB.max.x) * 0.5f;
	position.y = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
	position.z = (playerAABB.min.z + playerAABB.max.z) * 0.5f;


	if (onEnemy) {
		position.y += 2.0f;
		onEnemy = false;
	}
	if (EnemyContral && collisionEnemy) {
		position.y -= 2.0f;
		collisionEnemy = false;
	}

	worldTransform_.translation_ = position;
	if (EnemyContral) {
		worldTransform_.translation_.y += 2.0f;
	}

	// ばね敵との衝突チェック
	CheckCollisionWithSprings();

	ImGui::Begin("player");
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x);
	ImGui::DragFloat3("aabbMax", &playerAABB.max.x);
	ImGui::DragFloat3("aabbMin", &playerAABB.min.x);
	ImGui::End();

	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

	cameraController_.Update(camera_, position);
}

void Player::CheckCollision() {
	if (!block_->IsActive()) {
		return;
	}

	AABB blockAABB = block_->GetAABB();
			  
	switch (currentState) {
	case State::Normal:
		if (IsCollisionAABB(playerAABB, blockAABB)) {
			// worldTransform_.translation_ -= velocity; // 速度分だけ戻す
			ResolveAABBCollision(playerAABB, blockAABB, velocityY_, onGround_);
		}
		break;
	case State::Bomb:
		if (IsCollisionAABB(playerAABB, blockAABB)) {
			// worldTransform_.translation_ -= velocity; // 速度分だけ戻す
			ResolveAABBCollision(playerAABB, blockAABB, velocityY_, onGround_);
		}

		for (Bom* bom : cannonEnemy->GetBom()) {

			AABB bomAABB = bom->GetAABB();

			if (IsCollisionAABB(bomAABB, blockAABB) && cannonEnemy->GetPlayerCtrl()) {
				block_->SetActive(false);
			}
		}
		break;
	case State::Ghost:
		break;
	}

}

void Player::DrawUI() {
	ImGui::Begin("Player State");

	const char* stateNames[] = {"Normal", "Bomb", "Ghost"};
	ImGui::Text("Current State: %s", stateNames[static_cast<int>(currentState)]);

	ImGui::End();
}


Vector3 Player::GetWorldPosition() {

	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_, textureHandle); }

void Player::SetEnemyList(const std::vector<Enemy*>& enemies) { enemyList_ = enemies; }

//// ★ 新しく追加：ドアとの衝突解決処理
//void Player::ResolveCollisionWithDoor(const AABB& doorAABB) {
//	AABB currentAABB = GetAABB();
//	ResolveAABBCollision(currentAABB, doorAABB, velocityY_, onGround_);
//	position.x = (currentAABB.min.x + currentAABB.max.x) * 0.5f;
//	position.y = (currentAABB.min.y + currentAABB.max.y) * 0.5f;
//	position.z = (currentAABB.min.z + currentAABB.max.z) * 0.5f;
//	worldTransform_.translation_ = position;
//}

void Player::CheckCollisionWithSprings() {
	for (auto* springEnemy : springEnemies_) {
		AABB springAABB = springEnemy->GetAABB();

		if (IsCollisionAABB(playerAABB, springAABB) && !EnemyContral) {
			// Resolve collision
			ResolveAABBCollision(playerAABB, springAABB, velocityY_, onGround_);

			// If player is landing on top of the spring
			if (velocityY_ <= 0 && playerAABB.min.y >= springAABB.max.y - 0.2f) {
				// Apply the jump boost (much higher than normal jump)
				velocityY_ = 0.6f * springEnemy->GetJumpBoost();
				onGround_ = false;
				springEnemy->Compress(); // Trigger visual feedback
			}
		}
	}
}
