#include "Player.h"
#ifdef _DEBUG
#include "imgui.h"
#endif


#include <algorithm>
#include <iostream>

Player::Player() {}

Player::~Player() { delete PlayerModel_; }

void Player::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("player", true);
	worldTransform_.translation_ = position;
}

void Player::SetObstacleList(const std::vector<AABB>& obstacles) { obstacleList_.insert(obstacleList_.end(), obstacles.begin(), obstacles.end()); }

void Player::AddObstacle(const AABB& obstacle) { obstacleList_.push_back(obstacle); }

void Player::Update() {
	// キーボードとGamePad左スティックの入力を合算して移動処理する
	float keyboardSpeed = 0.55f;
	Vector3 inputVec = {0.0f, 0.0f, 0.0f};

	// キーボード入力 (WASD)
	if (Input::GetInstance()->PushKey(DIK_W)) {
		inputVec.z += keyboardSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		inputVec.z -= keyboardSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		inputVec.x -= keyboardSpeed;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		inputVec.x += keyboardSpeed;
	}

	// GamePad左スティック入力
	const float deadZone = 0.2f;
	if (Input::GetInstance()->GetJoystickState(0, state)) {
		float gpX = static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f;
		float gpZ = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;
		if (fabs(gpX) < deadZone)
			gpX = 0.0f;
		if (fabs(gpZ) < deadZone)
			gpZ = 0.0f;
		inputVec.x += gpX;
		inputVec.z += gpZ;
	}

	// 入力があれば正規化してスピード分移動
	if (Length(inputVec) > 0) {
		Vector3 move = Normalize(inputVec) * speed; // ※speedはメンバ変数等で定義済みとする
		move = TransformNormal(move, worldTransform_.matWorld_);
		position.x += move.x;
		position.z += move.z;
	}

	// 状態切替
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		currentState = State::Normal;
	}
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		currentState = State::Bomb;
	}
	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		currentState = State::Ghost;
	}
	
	//コントローラとキーボード両方で回さないようにするフラグ
	bool isKeyBorad = false;

	// キーボードによるカメラ回転X
	if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		cameraYaw -= 2.5f;
		isKeyBorad = true;
	}
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		cameraYaw += 2.5f;
		isKeyBorad = true;
	} 

	// キーボードによるカメラ回転Y
	if (Input::GetInstance()->PushKey(DIK_DOWN)) {
		cameraPitch += 1.5f;
		isKeyBorad = true;
	}
	if (Input::GetInstance()->PushKey(DIK_UP)) {
		cameraPitch -= 1.5f;
		isKeyBorad = true;
	}

	// GamePad右スティックによるカメラ回転処理
	float xCamera = 0.0f, zCamera = 0.0f;
	if (Input::GetInstance()->GetJoystickState(0, state) && !isKeyBorad) {

		// 右スティックの入力
		xCamera = static_cast<float>(state.Gamepad.sThumbRX) / 32768.0f; // -1.0f～1.0f
		 zCamera = static_cast<float>(state.Gamepad.sThumbRY) / 32768.0f; // -1.0f～1.0f

		// デッドゾーン処理
		if (abs(xCamera) < deadZone) {
			xCamera = 0.0f;
		if (fabs(zCamera) < deadZone)
			zCamera = 0.0f;
		}

		//カメラ向き
		//Y軸
		cameraYaw += xCamera * 2.5f;
		//X軸
		cameraPitch += zCamera * 2.5f;
	
	}

	cameraPitch = std::clamp(cameraPitch, 10.0f, 60.0f);
	
	cameraController_.SetPitch(cameraPitch);
	cameraController_.SetYaw(cameraYaw);
	worldTransform_.rotation_.y = -(cameraYaw * (3.14159265f / 180.0f));

	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

	// ジャンプ・移動時の各種処理
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

	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && onGround_ && EnemyContral) {
		velocityY_ = 0.0f;
		EnemyContral = false;
		onEnemy = true;
	} else if (Input::GetInstance()->TriggerKey(DIK_K) && onGround_ && EnemyContral) {
		velocityY_ = 0.0f;
		EnemyContral = false;
		onEnemy = true;
	}

	// 重力と垂直移動
	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	// プレイヤーのAABB更新
	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f;
	playerAABB.min = {position.x - halfW, position.y - halfH, position.z - halfD};
	playerAABB.max = {position.x + halfW, position.y + halfH, position.z + halfD};

	// 障害物との衝突解決
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

	// キャノン敵との衝突処理
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
		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {
			cannonEnemy->PlayerFire();
		} else if (Input::GetInstance()->TriggerKey(DIK_J)) {
			cannonEnemy->PlayerFire();
		}
	} else {
		cannonEnemy->ReMove(worldTransform_.translation_);
	}

	CheckCollision();

	for (SpringEnemy* springEnemy : springEnemies_) {
		AABB springAABB = springEnemy->GetAABB();

		if (IsCollisionAABB(playerAABB, springAABB) && !EnemyContral) {
			ResolveAABBCollision(playerAABB, springAABB, velocityY_, onGround_);
			if (isTransfar && (playerAABB.min.y >= springAABB.max.y)) {
				springEnemy->ContralPlayer();
				EnemyContral = true;
				collisionEnemy = true;
			}
		}

		if (EnemyContral && springEnemy->GetPlayerCtrl()) {
			springEnemy->SetParent(&worldTransform_);
		} else {
			springEnemy->ReMove(worldTransform_.translation_);
		}
	}


	// ドアとの衝突処理
	if (IsCollisionAABB(playerAABB, doorAABB) && !isOpenDoor) {
		ResolveAABBCollision(playerAABB, doorAABB, velocityY_, onGround_);
	}

	// 敵との衝突処理
	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		enemyAABB = (*it)->GetAABB();
		if (IsCollisionAABB(playerAABB, enemyAABB) && !EnemyContral) {

			//真上に乗れて、横は透ける
			Vector3 overlap = GetOverlapAmount(playerAABB,enemyAABB);
			if (overlap.y < overlap.x && overlap.y < overlap.z) {
				float playerCenterY = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
				float obstacleCenterY = (enemyAABB.min.y + enemyAABB.max.y) * 0.5f;
				float push = (playerCenterY < obstacleCenterY) ? -overlap.y : overlap.y;
				playerAABB.min.y += push;
				playerAABB.max.y += push;
				// 上向きの押し戻しなら着地判定を立てる
				if (push > 0.0f) {
					velocityY_ = 0.0f;
					onGround_ = true;
				}
			}

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

	// AABBの中心を基に位置を更新
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

#ifdef _DEBUG
	ImGui::Begin("player");
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x);
	ImGui::DragFloat3("aabbMax", &playerAABB.max.x);
	ImGui::DragFloat3("aabbMin", &playerAABB.min.x);
	ImGui::End();
#endif

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

#ifdef _DEBUG

	ImGui::Begin("Player State");

	const char* stateNames[] = {"Normal", "Bomb", "Ghost"};
	ImGui::Text("Current State: %s", stateNames[static_cast<int>(currentState)]);

	ImGui::End();

#endif // _DEBUG
  
}

void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }

void Player::SetEnemyList(const std::vector<Enemy*>& enemies) { enemyList_ = enemies; }

//// ★ 新しく追加：ドアとの衝突解決処理
// void Player::ResolveCollisionWithDoor(const AABB& doorAABB) {
//	AABB currentAABB = GetAABB();
//	ResolveAABBCollision(currentAABB, doorAABB, velocityY_, onGround_);
//	position.x = (currentAABB.min.x + currentAABB.max.x) * 0.5f;
//	position.y = (currentAABB.min.y + currentAABB.max.y) * 0.5f;
//	position.z = (currentAABB.min.z + currentAABB.max.z) * 0.5f;
//	worldTransform_.translation_ = position;
// }

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

void Player::SetState(State newState) {
	currentState = newState;
}