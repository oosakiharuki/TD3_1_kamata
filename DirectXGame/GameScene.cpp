#include "GameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
    delete player_;
    for (auto obj : gameObjects_) {
        delete obj;
    }
}

void GameScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    input_ = Input::GetInstance();
    audio_ = Audio::GetInstance();

    viewProjection_.Initialize();

    Model* modelPlayer = Model::Create();
    player_ = new Player();
    player_->Init(modelPlayer, &viewProjection_);

    Model* modelEnemy = Model::Create();
    for (int i = 0; i < 1; i++) {
        GameObject* obj = new GameObject(modelEnemy);
        obj->transform_.translation_ = Vector3(i * 2.0f, 0, 0);
        gameObjects_.push_back(obj);
    }
}

void GameScene::Update() {
    player_->Update();
    for (auto obj : gameObjects_) {
        obj->Update();
    }

    CheckCollisions();

    // プレイヤーの状態をキー入力で変更
    if (Input::GetInstance()->PushKey(DIK_1)) {
        player_->SetState(PlayerState::Normal);
    }
    if (Input::GetInstance()->PushKey(DIK_2)) {
        player_->SetState(PlayerState::Bom);
    }
    if (Input::GetInstance()->PushKey(DIK_3)) {
        player_->SetState(PlayerState::Ghost);
    }

    // ImGui で Player の状態を表示
    ImGui::Begin("Player Status");
    ImGui::Text("Player State: %s",
        player_->GetState() == PlayerState::Normal ? "Normal" :
        player_->GetState() == PlayerState::Bom ? "Bom" :
        "Ghost");
    ImGui::End();
}

void GameScene::Draw() {
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // 背景
    Sprite::PreDraw(commandList);
    Sprite::PostDraw();

    // モデル
    Model::PreDraw(commandList);
    player_->Draw();
    for (auto obj : gameObjects_) {
        obj->Draw(viewProjection_);
    }
    Model::PostDraw();

    // UI
    Sprite::PreDraw(commandList);
    Sprite::PostDraw();
}

void GameScene::CheckCollisions() {
    // プレイヤーの状態を取得
    PlayerState playerState = player_->GetState();

    for (size_t i = 0; i < gameObjects_.size(); i++) {
        for (size_t j = i + 1; j < gameObjects_.size(); j++) {
            // 一方または両方のオブジェクトが非アクティブならスキップ
            if (!gameObjects_[i]->isActive || !gameObjects_[j]->isActive) {
                continue;
            }

            // AABB衝突判定
            if (gameObjects_[i]->GetAABB().CheckCollision(gameObjects_[j]->GetAABB())) {
                if (playerState == PlayerState::Bom) {
                    // Bom状態なら片方のオブジェクトを消す
                    gameObjects_[j]->isActive = false;
                }
                else if (playerState == PlayerState::Ghost) {
                    // Ghost状態なら衝突を無視してすり抜ける
                    continue;
                }
                else {
                    // Normal状態では衝突が発生し、通常の処理を行う
                    // 例えば、オブジェクト同士の反発などの処理を行う
                    // 反発処理やダメージ処理をここに追加することができます
                }
            }
        }
    }
}