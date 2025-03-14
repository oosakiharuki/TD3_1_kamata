#include "GameScene.h"
#include "AABB.h"
#include <algorithm>
#include <vector>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;
	delete modelGround_;
	delete mapChipField_;

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

	// 床の障害物リストの作成（床全体の AABB）
	AddObstacle(floorObstacles_, {-200.0f, -5.5f, -200.0f}, {200.0f, 0.0f, 200.0f});

	// 座標方式のCSVを使用するための変更
	// MapChipField の初期化
	mapChipField_ = new MapChipField();
	mapChipField_->LoadCoordinateCsv("./Resources/map/map.csv"); // LoadMapChipCsvではなくLoadCoordinateCsvを使用

	// MapChipRenderer の初期化
	mapChipRenderer_ = new MapChipRenderer();
	if (!mapChipRenderer_->Init(&camera_, "./Resources/map/map.csv")) {
		// 読み込み失敗時の処理
		// エラーハンドリングを追加
		delete mapChipRenderer_;
		mapChipRenderer_ = nullptr;
	}

	// 座標方式のMapChipFieldから障害物リストを生成
	std::vector<AABB> tileObstacles;
	const std::vector<BlockDataCoord>& blocks = mapChipField_->GetBlocks();

	// 座標方式で読み込んだブロック情報から障害物を生成
	for (const auto& block : blocks) {
		// ブランク（空白）以外のブロックのみ障害物として追加
		if (block.type != MapChipType::kBlank) {
			AABB obstacle;
			// ブロックの中心座標はposition、サイズは一律1.0fと仮定
			obstacle.min = {block.position.x - 0.5f, block.position.y - 0.5f, block.position.z - 0.5f};
			obstacle.max = {block.position.x + 0.5f, block.position.y + 0.5f, block.position.z + 0.5f};
			tileObstacles.push_back(obstacle);
		}
	}

	// Enemy の生成と初期化（障害物リストに床障害物とマップチップから生成した障害物をセット）
	for (int i = 0; i < 5; ++i) {
		Enemy* enemy = new Enemy();
		enemy->Init(&camera_);
		enemy->SetTarget(player_);
		for (const auto& obstacles : floorObstacles_) {
			enemy->SetObstacleList(obstacles);
		}
		enemy->SetObstacleList(tileObstacles);
		enemyList_.push_back(enemy);
	}

	// 各 Enemy の初期位置設定
	if (!enemyList_.empty())
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
	for (const auto& obstacles : floorObstacles_) {
		player_->SetObstacleList(obstacles);
	}
	player_->SetObstacleList(tileObstacles);

	// Ground の生成・初期化
	modelGround_ = new Ground();
	modelGround_->Init(&camera_);
}

void GameScene::Update() {
	player_->Update();
	// MapChipField は静的なマップなので Update 処理は不要

	// Enemy の更新と不要な Enemy の削除・衝突判定
	 for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		(*it)->Update();
		if (std::find(player_->enemyList_.begin(), player_->enemyList_.end(), *it) == player_->enemyList_.end()) {
			delete *it;
			it = enemyList_.erase(it);
		} else {
			if ((*it)->CheckCollisionWithPlayer()) {
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

	Model::PreDraw(commandList);
	player_->Draw();
	modelGround_->Draw();

	mapChipRenderer_->Draw();

	// Enemy の描画
	 for (auto enemy : enemyList_) {
		enemy->Draw();
	 }
	Model::PostDraw();

	Sprite::PreDraw(commandList);
	Sprite::PostDraw();
}

void GameScene::AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max) {
	AABB obstacle;
	obstacle.min = min;
	obstacle.max = max;
	if (allObstacles.empty() || allObstacles.back().size() >= 100) {
		allObstacles.emplace_back();
	}
	allObstacles.back().push_back(obstacle);
}

void GameScene::SpawnEnemy(const Vector3& position) {
	Enemy* enemy = new Enemy();
	enemy->Init(&camera_);
	enemy->SetTarget(player_);
	enemy->SetPosition(position);
	for (const auto& obstacles : floorObstacles_) {
		enemy->SetObstacleList(obstacles);
	}
	// 生成時もマップチップから生成した障害物をセット
	std::vector<AABB> tileObstacles;
	uint32_t vert = mapChipField_->GetNumBlockVirtical();
	uint32_t horz = mapChipField_->GetNumBlockHorizontal();
	for (uint32_t y = 0; y < vert; ++y) {
		for (uint32_t x = 0; x < horz; ++x) {
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(x, y);
			if (type != MapChipType::kBlank) {
				Vector3 pos = mapChipField_->GetMapChipPostionByIndex(x, y);
				AABB obstacle;
				obstacle.min = {pos.x - 0.5f, pos.y - 0.5f, -0.5f};
				obstacle.max = {pos.x + 0.5f, pos.y + 0.5f, 0.5f};
				tileObstacles.push_back(obstacle);
			}
		}
	}
	enemy->SetObstacleList(tileObstacles);
	enemyList_.push_back(enemy);
	player_->SetEnemyList(enemyList_);
}
