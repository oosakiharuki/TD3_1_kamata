#include "GameScene.h"
#include <vector>

GameScene::GameScene() {}

GameScene::~GameScene() {
	// 先にプレイヤーと地面のリジッドボディを物理ワールドから削除
	if (player_) {
		physicsManager_->RemoveRigidBody(player_->GetRigidBody());
	}
	if (modelGround_) {
		physicsManager_->RemoveRigidBody(modelGround_->GetRigidBody());
	}

	delete player_;
	delete modelGround_;
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	camera_.Initialize();

	// Bullet物理エンジンのインスタンスを取得
	physicsManager_ = BulletPhysicsManager::GetInstance();

	// 重力を設定
	//physicsManager_->GetCollisionWorld()->setGravity(btVector3(0, -9.8f, 0));

	// Ground の生成・初期化（地面を先に作成して追加）
	modelGround_ = new Ground();
	modelGround_->Init(&camera_);
	physicsManager_->AddRigidBody(modelGround_->GetRigidBody());

	// Player の生成と初期化
	player_ = new Player();
	player_->Init(&camera_);
	physicsManager_->AddRigidBody(player_->GetRigidBody());
}

void GameScene::Update() {
	// 物理エンジンの更新
	physicsManager_->Update();

	// プレイヤーの更新
	player_->Update();

	// 地面の更新
	modelGround_->Update();
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	// モデル描画
	Model::PreDraw(commandList);
	player_->Draw();
	modelGround_->Draw();
	Model::PostDraw();

	// UI描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();
}