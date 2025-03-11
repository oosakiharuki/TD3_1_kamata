#include "Player.h"
#include "imgui.h"
#include <iostream>
#include <algorithm>

#include <KamataEngine.h>

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

	float x = 0, z = 0;

	Input::GetInstance()->GetJoystickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

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
		position.x += x * speed;
		position.z += z * speed;
	}
	
	switch (controler) {
	case Controler::player:

		if (!onGround_) {
			// のりうつるときの処理
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !EnemyContral) {
				velocityY_ -= 1.2f;
				isTransfar = true;
			} else if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !EnemyContral) {
				velocityY_ -= 1.2f;
				isTransfar = true;
			}
		} else {
			isTransfar = false;
		}
		break;
	case Controler::enemyTransfar:

		// if (onEnemy || position.y <= 2.0f) {
		//	onGround_ = true;
		// } else {
		//	onGround_ = false;
		// }

		break;
	default:
		break;
	};


	// ジャンプ処理
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && onGround_) {
		velocityY_ = 0.3f;
		onGround_ = false;
	}
	else if (Input::GetInstance()->TriggerKey(DIK_SPACE) && onGround_) {
		velocityY_ = 0.3f;
		onGround_ = false;
	}

	//position.x += x * speed;
	//position.z += z * speed;

		
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && onGround_) {
		velocityY_ = 0.0f;
		EnemyContral = false;
		onEnemy = true;
		controler = Controler::player;

	} else if (Input::GetInstance()->TriggerKey(DIK_K) && onGround_) {
		velocityY_ = 0.0f;
		EnemyContral = false;
		onEnemy = true;
		controler = Controler::player;
	}

	// 重力処理
	float gravity = 0.01f;
	velocityY_ -= gravity;
	position.y += velocityY_;

	// プレイヤーのAABB作成（例：幅1.0, 高さ2.0, 奥行1.0）
	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f;
	
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


    // Enemyとの衝突判定
	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		enemyAABB = (*it)->GetAABB();

		if (IsCollisionAABB(playerAABB, enemyAABB) && !EnemyContral) {
			// 衝突時の処理（例：リストから削除）
			// it = enemyList_.erase(it);			
			ResolveAABBCollision(playerAABB, enemyAABB, velocityY_, onGround_);
	
			// 頭からしか入れなくする
			if (isTransfar &&(playerAABB.min.y >= enemyAABB.max.y)) {
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

	// 衝突解決後のAABB中心をプレイヤー座標に反映
	position.x = (playerAABB.min.x + playerAABB.max.x) * 0.5f;
	position.y = (playerAABB.min.y + playerAABB.max.y) * 0.5f;
	position.z = (playerAABB.min.z + playerAABB.max.z) * 0.5f;

	/*/
	// Enemyとの衝突判定
	for (const auto& enemy : enemyList_) {
		AABB enemyAABB = enemy->GetAABB();
		if (IsCollisionAABB(playerAABB, enemyAABB)) {
			// 衝突時の処理をここに記述
		}
	}
	/*/

	if (onEnemy) {
		position.y += 2.0f; // 敵の上に乗るようにする
		onEnemy = false;
	}
	if (EnemyContral && collisionEnemy) {
		position.y -= 2.0f;
		collisionEnemy = false;
	}

	worldTransform_.translation_ = position;

	if (EnemyContral) {
		worldTransform_.translation_.y += 2.0f; // 敵の高さを足す
	}

	ImGui::Begin("test");
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x);
	ImGui::DragFloat3("aabbMax", &playerAABB.max.x);
	ImGui::DragFloat3("aabbMin", &playerAABB.min.x);
	ImGui::End();

	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

	cameraController_.Update(camera_, position);
}


void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_,  textureHandle); }

void Player::SetEnemyList(const std::vector<Enemy*>& enemies) { enemyList_ = enemies; }