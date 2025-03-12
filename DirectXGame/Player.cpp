#include "Player.h"
#include "BulletPhysicsManager.h"
#include <KamataEngine.h>

// カスタム接触コールバッククラス
class ContactCallback : public btCollisionWorld::ContactResultCallback {
public:
	ContactCallback(const btRigidBody* body) : btCollisionWorld::ContactResultCallback(), body_(body), hasFloorContact_(false), hasSideContact_(false) {}

	virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0, int partId0, int index0, const btCollisionObjectWrapper* colObj1, int partId1, int index1) override {
		// 自分自身の剛体との接触は無視
		if (colObj0->getCollisionObject() != body_ && colObj1->getCollisionObject() != body_)
			return 0;

		// 接触法線ベクトルを取得
		btVector3 normal = cp.m_normalWorldOnB;

		// プレイヤーオブジェクトが1番目の場合、法線の向きを反転
		if (colObj1->getCollisionObject() == body_) {
			normal = -normal;
		}

		// 法線のY成分が大きい場合（閾値0.7は約45度）は床との接触
		if (normal.y() > 0.7) {
			hasFloorContact_ = true;
			floorContactNormal_ = normal;
			floorContactDistance_ = cp.getDistance();
		}
		// それ以外は側面との接触
		else {
			hasSideContact_ = true;
			sideContactNormal_ = normal;
			sideContactDistance_ = cp.getDistance();
		}

		return 0;
	}

	bool hasFloorContact() const { return hasFloorContact_; }
	bool hasSideContact() const { return hasSideContact_; }
	const btVector3& getFloorContactNormal() const { return floorContactNormal_; }
	const btVector3& getSideContactNormal() const { return sideContactNormal_; }
	btScalar getFloorContactDistance() const { return floorContactDistance_; }
	btScalar getSideContactDistance() const { return sideContactDistance_; }

private:
	const btRigidBody* body_;
	bool hasFloorContact_;
	bool hasSideContact_;
	btVector3 floorContactNormal_;
	btVector3 sideContactNormal_;
	btScalar floorContactDistance_;
	btScalar sideContactDistance_;
};

Player::Player() {}

Player::~Player() {
	delete PlayerModel_;

	// Bullet物理オブジェクトの解放
	if (rigidBody_) {
		if (rigidBody_->getMotionState()) {
			delete rigidBody_->getMotionState();
		}
		delete rigidBody_;
		rigidBody_ = nullptr;
	}

	if (collisionShape_) {
		delete collisionShape_;
		collisionShape_ = nullptr;
	}
}

void Player::Init(Camera* camera) {
	camera_ = camera;
	worldTransform_.Initialize();
	// "cube" モデルを読み込み
	PlayerModel_ = Model::CreateFromOBJ("cube", true);
	position = Vector3(0, 5, 0); // 初期位置を高めに設定して落下させる
	worldTransform_.translation_ = position;

	// Bullet物理オブジェクトの作成（キューブ形状より少し小さく）
	collisionShape_ = new btBoxShape(btVector3(0.4f, 0.4f, 0.4f)); // キューブよりやや小さく

	// 剛体の初期トランスフォーム
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.x, position.y, position.z));

	// 剛体の質量と慣性
	btScalar mass = 1.0f; // プレイヤーの質量
	btVector3 localInertia(0, 0, 0);
	collisionShape_->calculateLocalInertia(mass, localInertia);

	// モーションステートの作成
	motionState_ = new btDefaultMotionState(transform);

	// 剛体の作成
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState_, collisionShape_, localInertia);
	rbInfo.m_friction = 0.1f;    // 摩擦係数を下げる（滑りやすくする）
	rbInfo.m_restitution = 0.0f; // 反発係数（バウンドを防ぐため0に）
	rigidBody_ = new btRigidBody(rbInfo);

	// 重力を設定
	rigidBody_->setGravity(btVector3(0, -9.8f, 0));

	// 回転をロック（キャラクターは回転しない）
	rigidBody_->setAngularFactor(btVector3(0, 0, 0));

	// 衝突時のコールバックを有効にする
	rigidBody_->setContactProcessingThreshold(0.0f);
	rigidBody_->setCcdMotionThreshold(0.1f);
	rigidBody_->setCcdSweptSphereRadius(0.4f);

	// 衝突マージンを小さく設定
	collisionShape_->setMargin(0.01f);

	// 初期化
	onGround_ = false;
	velocityY_ = 0.0f;
	floorContact_.isContact = false;
	sideContact_.isContact = false;

	// フィールド範囲（仮の値、実際のフィールドサイズに合わせて調整）
	fieldMinX = -20.0f;
	fieldMaxX = 20.0f;
	fieldMinZ = -20.0f;
	fieldMaxZ = 20.0f;
	fieldHeight = 0.0f; // 地面の高さ
}

void Player::Update() {
	// NULL チェック
	if (!rigidBody_ || !BulletPhysicsManager::GetInstance() || !BulletPhysicsManager::GetInstance()->GetCollisionWorld()) {
		return;
	}

	// 接触判定の更新
	UpdateContactCheck();

	// 前フレームの位置を保存
	Vector3 previousPosition = position;

	float moveSpeed = 0.5f; // 元の移動速度に合わせる

	// 移動制御（接地状態に関わらず移動可能）
	bool moved = false;

	// WASD移動（直接位置を変更）
	if (Input::GetInstance()->PushKey(DIK_W)) {
		position.z += moveSpeed;
		moved = true;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		position.z -= moveSpeed;
		moved = true;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		position.x -= moveSpeed;
		moved = true;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		position.x += moveSpeed;
		moved = true;
	}

	// 接地状態による処理
	if (onGround_) {
		// 接地時は重力をゼロにする
		velocityY_ = 0.0f;

		// ジャンプ処理
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocityY_ = 0.3f; // 元のジャンプ力に合わせる
			onGround_ = false;
		}
	} else {
		// 空中時は重力を適用
		float gravity = 0.01f; // 元の重力値に合わせる
		velocityY_ -= gravity;
	}

	// Y座標の更新（重力・ジャンプの影響）
	position.y += velocityY_;

	// フィールド外判定 - groundオブジェクトの実際のサイズに基づいて設定
	bool isOutOfField = (position.x < -14.7f || position.x > 14.7f || position.z < -55.95f || position.z > 57.1f);

	// フィールド外処理
	if (isOutOfField) {
		// フィールド外に出た場合の処理
		// オプション1: フィールド内に押し戻す
		if (position.x < -14.7f)
			position.x = -14.7f;
		if (position.x > 14.7f)
			position.x = 14.7f;
		if (position.z < -55.95f)
			position.z = -55.95f;
		if (position.z > 57.1f)
			position.z = 57.1f;

		// オプション2: 一定高度以下で強制的に落下させる
		if (position.y <= 0.55f) {
			velocityY_ -= 0.02f; // 通常より強い重力
		}
	}

	// 地面すり抜け防止 - 最低高度の設定
	if (!isOutOfField && position.y < 0.55f) {
		position.y = 0.55f;
		velocityY_ = 0.0f;
		onGround_ = true;
	}

	// 側面接触時の位置調整（移動した場合のみ）
	if (sideContact_.isContact && moved) {
		// 壁からの押し戻し
		btVector3 normal = sideContact_.contactNormal;
		float penetrationDepth = sideContact_.contactDistance;

		// 移動方向を計算
		Vector3 moveDirection = position - previousPosition;
		btVector3 moveDir(moveDirection.x, 0, moveDirection.z);

		if (moveDir.length() > 0.001f) {
			moveDir.normalize();

			// 法線と移動方向の内積を計算
			float dot = moveDir.dot(normal);

			// 壁に向かって移動している場合のみ調整（内積が負の場合）
			if (dot < 0) {
				// 壁に沿った移動のみを許可
				btVector3 tangent = moveDir - normal * dot;
				if (tangent.length() > 0.001f) {
					tangent.normalize();

					// 移動量を再計算（壁に沿った方向のみ）
					float adjustedSpeed = moveSpeed * 0.8f; // 少し減速
					position.x = previousPosition.x + tangent.x() * adjustedSpeed;
					position.z = previousPosition.z + tangent.z() * adjustedSpeed;
				} else {
					// 完全に壁に向かっている場合は移動をキャンセル
					position.x = previousPosition.x;
					position.z = previousPosition.z;

					// めり込みを解消
					if (penetrationDepth < 0) {
						position.x += normal.x() * (-penetrationDepth + 0.05f);
						position.z += normal.z() * (-penetrationDepth + 0.05f);
					}
				}
			}
		}
	}

	// 強制フォールアウト判定 - 一定高度以下になったらリスポーン
	if (position.y < -20.0f) {
		position = Vector3(0, 5, 0);
		velocityY_ = 0.0f;
	}

	// 位置の更新を物理エンジンに反映
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.x, position.y, position.z));
	rigidBody_->getMotionState()->setWorldTransform(transform);
	rigidBody_->setCenterOfMassTransform(transform);

	// 速度をリセット（位置を直接操作するため）
	rigidBody_->setLinearVelocity(btVector3(0, 0, 0));

	// 世界行列の更新
	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();

	// ImGuiによる接地状態のデバッグ表示
	ImGui::Begin("Player Debug");
	ImGui::Checkbox("On Ground", &onGround_);
	ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ImGui::Text("Velocity Y: %.4f", velocityY_);
	ImGui::Text("Side Contact: %s", sideContact_.isContact ? "Yes" : "No");
	ImGui::Text("Field: %s", isOutOfField ? "Outside" : "Inside");
	ImGui::End();

	// カメラコントローラーの更新
	cameraController_.Update(camera_, position);
}

void Player::UpdateContactCheck() {
	// NULL チェック
	if (!rigidBody_ || !BulletPhysicsManager::GetInstance() || !BulletPhysicsManager::GetInstance()->GetCollisionWorld()) {
		return;
	}

	// 接触情報をリセット
	floorContact_.isContact = false;
	sideContact_.isContact = false;

	// フィールド外判定 - groundオブジェクトの実際のサイズに基づいて設定
	bool isOutOfField = (position.x < -14.7f || position.x > 14.7f || position.z < -55.95f || position.z > 57.1f);

	// フィールド外の場合も地面接触判定を行う（落下させるため）

	// レイキャストによる地面接触判定 - レイの長さを調整
	{
		btTransform trans;
		rigidBody_->getMotionState()->getWorldTransform(trans);
		btVector3 rayStart = trans.getOrigin();

		// レイの長さを拡大（0.6fから0.8fへ）して確実に検出
		float rayLength = 0.8f;
		btVector3 rayEnd = rayStart - btVector3(0, rayLength, 0);

		btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);
		BulletPhysicsManager::GetInstance()->GetCollisionWorld()->rayTest(rayStart, rayEnd, rayCallback);

		if (rayCallback.hasHit()) {
			onGround_ = true;
			floorContact_.isContact = true;
			floorContact_.contactNormal = rayCallback.m_hitNormalWorld;
			floorContact_.contactDistance = rayCallback.m_closestHitFraction * rayLength;

			// 接触面との距離に基づいて位置調整
			if (floorContact_.contactDistance < 0.55f) {
				// めり込みを防ぐため、地面の上に押し上げる
				position.y = rayStart.y() - floorContact_.contactDistance + 0.55f;
				velocityY_ = 0.0f;
			}
		} else {
			// サブレイキャストを追加（中心だけでなく四隅も検査）
			bool hitAny = false;

			// プレイヤーの四隅からのレイキャスト
			float offset = 0.3f; // プレイヤーのサイズの約75%
			btVector3 corners[4] = {btVector3(offset, 0, offset), btVector3(offset, 0, -offset), btVector3(-offset, 0, offset), btVector3(-offset, 0, -offset)};

			for (int i = 0; i < 4; i++) {
				btVector3 cornerRayStart = rayStart + corners[i];
				btVector3 cornerRayEnd = cornerRayStart - btVector3(0, rayLength, 0);

				btCollisionWorld::ClosestRayResultCallback cornerRayCallback(cornerRayStart, cornerRayEnd);
				BulletPhysicsManager::GetInstance()->GetCollisionWorld()->rayTest(cornerRayStart, cornerRayEnd, cornerRayCallback);

				if (cornerRayCallback.hasHit()) {
					hitAny = true;
					floorContact_.isContact = true;
					floorContact_.contactNormal = cornerRayCallback.m_hitNormalWorld;
					floorContact_.contactDistance = cornerRayCallback.m_closestHitFraction * rayLength;

					// 四隅のどれかが接触していれば接地している
					onGround_ = true;

					// 接触面との距離に基づいて位置調整
					if (floorContact_.contactDistance < 0.55f) {
						position.y = cornerRayStart.y() - floorContact_.contactDistance + 0.55f;
						velocityY_ = 0.0f;
					}

					break;
				}
			}

			if (!hitAny) {
				onGround_ = false;
			}
		}
	}

	// 補助的な接地判定（位置ベース）- groundの正確な高さを使用
	if (position.y <= 0.55f) {
		onGround_ = true;
		floorContact_.isContact = true;
		floorContact_.contactNormal = btVector3(0, 1, 0);

		// 地面をすり抜けないよう位置を修正
		position.y = 0.55f;
		velocityY_ = 0.0f;
	}

	// フィールド外の場合の処理
	if (isOutOfField) {
		// フィールド外では一定高度以下で落下継続
		if (position.y <= -10.0f) {
			// 一定以上落下したらリスポーン
			position = Vector3(0, 5, 0);
			velocityY_ = 0.0f;
		}
	}

	// 側面接触判定用の4方向レイキャスト
	{
		btTransform trans;
		rigidBody_->getMotionState()->getWorldTransform(trans);
		btVector3 origin = trans.getOrigin();

		// レイの長さを少し拡大
		float rayLength = 0.6f;
		btVector3 directions[4] = {
		    btVector3(rayLength, 0, 0),  // 右
		    btVector3(-rayLength, 0, 0), // 左
		    btVector3(0, 0, rayLength),  // 前
		    btVector3(0, 0, -rayLength)  // 後
		};

		for (int i = 0; i < 4; i++) {
			// レイキャストの高さを調整（複数の高さでチェック）
			float heights[3] = {0.1f, 0.3f, 0.5f};

			for (int h = 0; h < 3; h++) {
				btVector3 rayStart = origin + btVector3(0, heights[h], 0);
				btVector3 rayEnd = rayStart + directions[i];

				btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);
				BulletPhysicsManager::GetInstance()->GetCollisionWorld()->rayTest(rayStart, rayEnd, rayCallback);

				if (rayCallback.hasHit()) {
					sideContact_.isContact = true;
					sideContact_.contactNormal = rayCallback.m_hitNormalWorld;
					sideContact_.contactDistance = rayCallback.m_closestHitFraction * rayLength - 0.45f;
					break;
				}
			}

			if (sideContact_.isContact) {
				break;
			}
		}
	}
}

void Player::SyncPhysicsTransform() {
	// NULL チェック
	if (!rigidBody_) {
		return;
	}

	// 物理シミュレーションの結果を取得
	btTransform trans;
	rigidBody_->getMotionState()->getWorldTransform(trans);

	// 位置を更新
	btVector3 pos = trans.getOrigin();
	position.x = pos.x();
	position.y = pos.y();
	position.z = pos.z();

	// ワールドトランスフォームの更新
	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void Player::Draw() { PlayerModel_->Draw(worldTransform_, *camera_); }