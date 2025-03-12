#include "Player.h"

void Player::Init(Model* model, Camera* viewProjection, Vector3& pos, Block* block) {
    model_ = model;
    viewProjection_ = viewProjection;
    this->block_ = block;  // ブロックを受け取る

    worldTransform.Initialize();
    worldTransform.translation_ = pos;
}

void Player::Update() {
    velocity = { 0.0f, 0.0f, 0.0f }; // 毎フレーム初期化

    Input::GetInstance()->GetJoystickState(0, state);
    Input::GetInstance()->GetJoystickStatePrevious(0, preState);

    float x = 0, z = 0;

    // ゲームパッドの入力
    if (Input::GetInstance()->GetJoystickState(0, state)) {
        x = static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f;
        z = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;

        const float deadZone = 0.2f;
        if (abs(x) < deadZone) { x = 0.0f; }
        if (abs(z) < deadZone) { z = 0.0f; }
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

    velocity.x += x * speed;
    velocity.z += z * speed;

    // キーボードの入力
    if (Input::GetInstance()->PushKey(DIK_A)) { velocity.x -= speed; }
    if (Input::GetInstance()->PushKey(DIK_D)) { velocity.x += speed; }
    if (Input::GetInstance()->PushKey(DIK_S)) { velocity.z -= speed; }
    if (Input::GetInstance()->PushKey(DIK_W)) { velocity.z += speed; }

    // ジャンプ処理
    if (IsJump) {
        velocity.y -= 0.1f;
    }
    else {
        velocity.y = 0.0f;
    }

    if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
        !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !IsJump) {
        IsJump = true;
        velocity.y = 1.5f;
    }

    // 地面に着地したらジャンプ終了
    if (worldTransform.translation_.y < 0.0f) {
        worldTransform.translation_.y = 0.0f;
        IsJump = false;
    }

    // 位置を更新
    worldTransform.translation_ += velocity;
    worldTransform.UpdateMatrix();
}

void Player::CheckCollision(Block* block) {
    if (!block->IsActive()) return;

    AABB playerAABB = {
        worldTransform.translation_ - Vector3(1.0f, 1.0f, 1.0f),
        worldTransform.translation_ + Vector3(1.0f, 1.0f, 1.0f)
    };

    AABB blockAABB = block->GetAABB();

    if (IsCollisionAABB(playerAABB, blockAABB)) {
        switch (currentState) {
        case State::Normal:
            worldTransform.translation_ -= velocity; // 速度分だけ戻す
            break;
        case State::Bomb:
            block->SetActive(false);
            break;
        case State::Ghost:
            break;
        }
    }
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