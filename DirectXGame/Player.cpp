#include "Player.h"

void Player::Init(Model* model, Camera* viewProjection, Vector3& pos, Block* block) {
    model_ = model;
    viewProjection_ = viewProjection;
    this->block_ = block;  // ブロックを受け取る

    worldTransform.Initialize();
    worldTransform.translation_ = pos;
}

void Player::Update() {
    float x = 0, z = 0;

    Input::GetInstance()->GetJoystickState(0, state);
    Input::GetInstance()->GetJoystickStatePrevious(0, preState);

    // ゲームパッドの入力処理（移動）
    if (Input::GetInstance()->GetJoystickState(0, state)) {
        x = static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f;
        z = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;

        const float deadZone = 0.2f;
        if (abs(x) < deadZone) x = 0.0f;
        if (abs(z) < deadZone) z = 0.0f;
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

    // キーボード入力での移動（WASD）
    if (Input::GetInstance()->PushKey(DIK_W)) {
        z = speed;  // Wキーで前進
    }
    if (Input::GetInstance()->PushKey(DIK_S)) {
        z = -speed;  // Sキーで後退
    }
    if (Input::GetInstance()->PushKey(DIK_A)) {
        x = -speed;  // Aキーで左移動
    }
    if (Input::GetInstance()->PushKey(DIK_D)) {
        x = speed;  // Dキーで右移動
    }

    // ジャンプ処理
    if (IsJump) {
        if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
            velocity.y -= 1.2f;
        }
        else {
            velocity.y -= 0.1f;
        }
    }
    else {
        velocity.y = 0.0f;
    }

    // ジャンプの開始
    if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !IsJump) {
        IsJump = true;
        velocity.y = 1.5f;
    }

    // Y軸の衝突判定（地面と衝突）
    worldTransform.translation_.y += velocity.y;
    if (worldTransform.translation_.y < 0.0f) {
        worldTransform.translation_.y = 0.0f;
        IsJump = false;
    }

    // X軸とZ軸の移動処理
    worldTransform.translation_.x += x * speed;
    worldTransform.translation_.z += z * speed;

    // 衝突判定：ブロックとの衝突をチェック
    AABB playerAABB;
    playerAABB.min = worldTransform.translation_ - Vector3(1.0f, 1.0f, 1.0f);
    playerAABB.max = worldTransform.translation_ + Vector3(1.0f, 1.0f, 1.0f);

    AABB blockAABB;
    blockAABB.min = block_->GetWorldTransform().translation_ - Vector3(1.0f, 1.0f, 1.0f);
    blockAABB.max = block_->GetWorldTransform().translation_ + Vector3(1.0f, 1.0f, 1.0f);

    if (IsCollisionAABB(playerAABB, blockAABB)) {
        // 衝突した場合、移動を停止
        if (x != 0) worldTransform.translation_.x -= x * speed;  // X軸で衝突した場合は移動を元に戻す
        if (z != 0) worldTransform.translation_.z -= z * speed;  // Z軸で衝突した場合は移動を元に戻す
    }

    // X, Y, Z位置の更新
    worldTransform.UpdateMatrix();
}

void Player::Draw() {
    model_->Draw(worldTransform, *viewProjection_);
}

void Player::DrawUI() {
    ImGui::Begin("Player State");

    const char* stateNames[] = { "Normal", "Bomb", "Ghost" };
    ImGui::Text("Current State: %s", stateNames[static_cast<int>(currentState)]);

    ImGui::End();
}