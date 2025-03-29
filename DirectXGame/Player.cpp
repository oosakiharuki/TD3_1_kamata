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
	block_ = new Block;
	ghostBlock_ = new GhostBlock;
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

	// コントローラとキーボード両方で回さないようにするフラグ
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

		// カメラ向き
		// Y軸
		cameraYaw += xCamera * 2.5f;
		// X軸
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
	if (IsCollisionAABB(playerAABB, cannonAABB)) {
		ResolveAABBCollision(playerAABB, cannonAABB, velocityY_, onGround_);
		if (isTransfar && (playerAABB.min.y >= cannonAABB.max.y) && !EnemyContral) {
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

	OnCollisions();


	for (SpringEnemy* springEnemy : springEnemies_) {
		AABB springAABB = springEnemy->GetAABB();

		if (IsCollisionAABB(playerAABB, springAABB)) {
			ResolveAABBCollision(playerAABB, springAABB, velocityY_, onGround_);
			if (isTransfar && (playerAABB.min.y >= springAABB.max.y) && !EnemyContral) {
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
		if (IsCollisionAABB(playerAABB, enemyAABB)) {

			// 真上に乗れて、横は透ける
			Vector3 overlap = GetOverlapAmount(playerAABB, enemyAABB);
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
			} else { //　横に当たったらダメージ
				isDamage = true;
			}

			if (isTransfar && (playerAABB.min.y >= enemyAABB.max.y) && !EnemyContral) {
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

	//　攻撃されたら
	CheckDamage();

	//ゴールの旗に当たったか
	CheckCollisionWithGoal(); 

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
	// ブロックリストが空の場合は処理を行わない
	if (blocks_.empty()) {
		return;
	}

	// 重複宣言を避けるため、1回だけ宣言する
	AABB mainBlockAABB = block_->GetAABB();
	AABB ghostBlockAABB = ghostBlock_->GetAABB();

	// 現在の状態に応じて各ブロックとの衝突判定を行う
	switch (currentState) {
	case State::Normal:
		// 通常状態：すべてのブロックと衝突判定
		for (Block* block : blocks_) {
			if (block && block->IsActive()) {
				// 変数名を変えて、重複を回避
				AABB currentBlockAABB = block->GetAABB();
				if (IsCollisionAABB(playerAABB, currentBlockAABB)) {
					ResolveAABBCollision(playerAABB, currentBlockAABB, velocityY_, onGround_);
				}
			}
		}
		if (ghostBlock_->IsActive() && IsCollisionAABB(playerAABB, ghostBlockAABB)) {
			ResolveAABBCollision(playerAABB, ghostBlockAABB, velocityY_, onGround_);
		}
		break;

	case State::Bomb:
		// 爆弾状態：衝突判定とブロック破壊
		for (Block* block : blocks_) {
			if (block && block->IsActive()) {
				// 変数名を変えて、重複を回避
				AABB currentBlockAABB = block->GetAABB();
				if (IsCollisionAABB(playerAABB, currentBlockAABB)) {
					ResolveAABBCollision(playerAABB, currentBlockAABB, velocityY_, onGround_);
				}

				// キャノン敵の弾との衝突判定
				for (Bom* bom : cannonEnemy->GetBom()) {
					AABB bomAABB = bom->GetAABB();
					if (IsCollisionAABB(bomAABB, currentBlockAABB) && cannonEnemy->GetPlayerCtrl()) {
						block->SetActive(false);
					}
				}
			}
		}
		break;

	case State::Ghost:
		// ゴースト状態：衝突判定なし
		break;
	}
}

void Player::DrawUI() {

#ifdef _DEBUG

	ImGui::Begin("Player State");

	const char* stateNames[] = {"Normal", "Bomb", "Ghost"};
	ImGui::Text("Current State: %s", stateNames[static_cast<int>(currentState)]);
	ImGui::DragFloat("Hp", &hp);

	ImGui::End();

#endif // _DEBUG
}

void Player::OnCollisions() {

	for (Bom* bom : cannonEnemy->GetBom()) {
		AABB bomAABB = bom->GetAABB();

		if (IsCollisionAABB(bomAABB, playerAABB) && !cannonEnemy->GetPlayerCtrl()) {
			isDamage = true;
			bom->OnCollision();
		}
	}
}

void Player::Draw() {
	// 点滅中で非表示の場合は描画しない
	if (isFlashing && !isVisible) {
		return;
	}

	// 通常の描画（テクスチャハンドルは使わない）
	PlayerModel_->Draw(worldTransform_, *camera_);
}

void Player::SetEnemyList(const std::vector<Enemy*>& enemies) { enemyList_ = enemies; }

void Player::SetSpringEnemies(const std::vector<SpringEnemy*>& springEnemies) { springEnemies_ = springEnemies; }

void Player::SetCannon(CannonEnemy* cannon) { cannonEnemy = cannon; }

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

		if (IsCollisionAABB(playerAABB, springAABB)) {
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


void Player::SetState(State newState) { currentState = newState; }

void Player::ClearObstacleList() {
	obstacleList_.clear(); // 当たり判定用の障害物リストをクリア
}

void Player::SetPosition(const Vector3& newPosition) {
	position = newPosition;
	worldTransform_.translation_ = position;
}


void Player::CheckCollisionWithGoal() {
	// ゴールが設定されていない場合は何もしない
	if (!goal_) {
		return;
	}

	AABB goalAABB = goal_->GetAABB();

	if (IsCollisionAABB(playerAABB, goalAABB)) {
		goal_->OnCollision();
	}
}

void Player::CheckDamage() {
	const float deltaTime = 1.0f / 60.0f;

	// 点滅処理の更新
	if (isFlashing) {
		flashTimer += deltaTime;

		// 0.3秒ごとに表示/非表示を切り替え
		if (flashTimer >= flashInterval) {
			isVisible = !isVisible; // 表示状態を反転
			flashTimer = 0.0f;      // タイマーリセット
		}

		// 点滅の継続時間（1.5秒）が終了したら
		coolTime -= deltaTime;
		if (coolTime <= 0.0f) {
			isFlashing = false;
			isVisible = true; // 必ず表示状態に戻す
			coolTime = 0.0f;
		}
	}

	// ダメージを受けたときの処理
	if (isDamage && !isFlashing) {
		hp--;
		isDamage = false;
		coolTime = flashDuration; // 1.5秒間の点滅時間
		isFlashing = true;
		flashTimer = 0.0f;
		isVisible = true;
	} else {
		isDamage = false;
	}
}
