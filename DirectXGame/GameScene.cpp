#include "GameScene.h"
#include "AABB.h"
#include <vector>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;
	delete modelGround_;
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	camera_.Initialize();

	// Player の生成と初期化
	player_ = new Player();
	player_->Init(&camera_);

	// 障害物リストの作成例
	std::vector<AABB> obstacles;

	// 例：壁のAABB（仮の座標・サイズ）
	AABB wall;
	wall.min = {5.0f, 0.0f, 3.0f};
	wall.max = {7.0f, 5.0f, 5.0f};
	obstacles.push_back(wall);

	// 例：床のAABB
	AABB floor;
	floor.min = {-10.0f, -0.5f, -10.0f};
	floor.max = {10.0f, 0.0f, 10.0f};
	obstacles.push_back(floor);

	// 障害物リストを Player にセット
	player_->SetObstacleList(obstacles);

	// Ground の生成・初期化
	modelGround_ = new Ground();
	modelGround_->Init(&camera_);
}

void GameScene::Update() { player_->Update(); }

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
