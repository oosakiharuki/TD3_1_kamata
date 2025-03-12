#include "GameScene.h"
#include "AABB.h"
#include <vector>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;
	delete modelGround_;
	for (auto enemy : enemyList_) {
		delete enemy;
	}
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	camera_.Initialize();

	// Player の生成と初期化
	textureHandle = TextureManager::GetInstance()->Load("uvChecker.png");
	player_ = new Player();
	player_->Init(&camera_, textureHandle);

	// 障害物リストの作成例
	AddObstacle(allObstacles_, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f});            // 例：壁のAABB
	AddObstacle(allObstacles_, {-200.0f, -5.5f, -200.0f}, {200.0f, 0.0f, 200.0f}); // 例：床のAABB全体の床
	AddObstacle(allObstacles_, {-10.0f, -0.5f, -10.0f}, {20.0f, 3.0f, 20.0f});     // 新しい足場AABB白
	AddObstacle(allObstacles_, {18.0f, -0.5f, -10.0f}, {37.5f, 6.0f, 20.0f});      // 新しい足場のAABB白

	AddObstacle(allObstacles_, {6.2f, -0.5f, -41.0f}, {19.5f, 3.5f, -27.3f}); // 宇宙模様の床

	// Enemyの生成と初期化
	for (int i = 0; i < 5; ++i) { // 例として5体のEnemyを生成
		Enemy* enemy = new Enemy();
		enemy->Init(&camera_);
		enemy->SetTarget(player_); // Playerの位置を設定
		for (const auto& obstacles : allObstacles_) {
			enemy->SetObstacleList(obstacles);
		}
		enemyList_.push_back(enemy);
	}

	// 各Enemyの初期位置を設定
	if (enemyList_.size() > 0)
		enemyList_[0]->SetPosition({-20.0f, 0.0f, -10.0f});
	if (enemyList_.size() > 1)
		enemyList_[1]->SetPosition({-10.0f, 0.0f, -10.0f});
	if (enemyList_.size() > 2)
		enemyList_[2]->SetPosition({20.0f, 0.0f, -20.0f});
	if (enemyList_.size() > 3)
		enemyList_[3]->SetPosition({-40.0f, 0.0f, -10.0f});
	if (enemyList_.size() > 4)
		enemyList_[4]->SetPosition({50.0f, 0.0f, -20.0f});

	player_->SetEnemyList(enemyList_);

	// 障害物リストを Player にセット
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	// Ground の生成・初期化
	modelGround_ = new Ground();
	modelGround_->Init(&camera_);
}

void GameScene::Update() {
	player_->Update();
	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		(*it)->Update();
		if (std::find(player_->enemyList_.begin(), player_->enemyList_.end(), *it) == player_->enemyList_.end()) {
			delete *it;
			it = enemyList_.erase(it);
		} else {
			// 衝突判定
			if ((*it)->CheckCollisionWithPlayer()) {
				// 衝突時の処理（移動を停止）
				(*it)->SetVelocity(Vector3(0, 0, 0));
			}
			++it;
		}
	}
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
	for (auto enemy : enemyList_) {
		enemy->Draw();
	}
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
