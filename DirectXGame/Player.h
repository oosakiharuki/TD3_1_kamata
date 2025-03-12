#pragma once
#include "3d/WorldTransform.h"
#include "math/Vector3.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "input/input.h"
#include "Block.h"  // 衝突判定用にブロックをインクルード
#include "AABB.h"
#include <2d/ImGuiManager.h> // ImGuiのヘッダーを追加

using namespace KamataEngine;

class Player {
public:
    enum class State {
        Normal, // 通常状態
        Bomb,   // ブロックを壊せる状態
        Ghost   // ブロックをすり抜ける状態
    };

    void Init(Model* model, Camera* viewProjection, Vector3& position, Block* block);
    void Update();
    void Draw();

    void DrawUI(); // UI描画用の関数を追加

    void CheckCollision(Block* block); // 衝突判定を追加

private:
    WorldTransform worldTransform;
    Camera* viewProjection_ = nullptr;
    Model* model_ = nullptr;
    Block* block_ = nullptr;  // 衝突判定用のブロックを保持

    State currentState = State::Normal; // 初期状態をNormalに設定

    Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // 速度ベクトル
    Vector3 position;                      // 現在の位置
    bool IsJump = false;

    XINPUT_STATE state, preState;
    const float speed = 0.2f; // 移動速度
};