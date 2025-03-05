#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "GameObject.h"
#include <vector>

class GameScene {
public:
    GameScene();
    ~GameScene();

    void Initialize();
    void Update();
    void Draw();

private:
    void CheckCollisions(); // AABB衝突チェック

    DirectXCommon* dxCommon_ = nullptr;
    Input* input_ = nullptr;
    Audio* audio_ = nullptr;
    WorldTransform worldTransform_;
    Camera viewProjection_;

    Player* player_ = nullptr;
    std::vector<GameObject*> gameObjects_; // 複数のオブジェクト管理
};