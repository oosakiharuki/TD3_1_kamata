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
	AddObstacle(allObstacles_, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f});            // 例：壁のAABB
	AddObstacle(allObstacles_, {-200.0f, -5.5f, -200.0f}, {200.0f, 0.0f, 200.0f}); // 例：床のAABB
	AddObstacle(allObstacles_, {-10.0f, -0.5f, -10.0f}, {20.0f, 3.0f, 20.0f});     // 新しい足場AABB
	AddObstacle(allObstacles_, {18.0f, -0.5f, -10.0f}, {37.5f, 6.0f, 20.0f});      // 新しい足場のAABB

	// 障害物リストを Player にセット
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

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

void GameScene::AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max) {
	AABB obstacle;
	obstacle.min = min;
	obstacle.max = max;
	if (allObstacles.empty() || allObstacles.back().size() >= 100) { // 100個の障害物を追加
		allObstacles.emplace_back();
	}
	allObstacles.back().push_back(obstacle);
}