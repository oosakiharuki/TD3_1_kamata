#include "GameScene.h"
#include "AABB.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;

	// MapLoaderの解放
	delete mapLoader_;

	// EnemyLoaderの解放
	delete enemyLoader_;

	delete stage;
	delete block_;
	delete modelBlock_;
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	camera_.Initialize();

	uint32_t texturehandle2 = TextureManager::GetInstance()->Load("Block.png");
	block_ = new Block();
	modelBlock_ = Model::Create();
	block_->Init(modelBlock_, &camera_, texturehandle2);

	// Player の生成と初期化
	textureHandle = TextureManager::GetInstance()->Load("uvChecker.png");
	player_ = new Player();
	player_->Init(&camera_, textureHandle);

	stage = Model::CreateFromOBJ("stage" + std::to_string(currentStage_), true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("space", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// **MapLoader の初期化**
	mapLoader_ = new MapLoader();
	std::string objectsFile = "Resources/objects" + std::to_string(currentStage_) + ".csv";
	if (mapLoader_->LoadMapData(objectsFile)) {
		mapLoader_->CreateObjects(&camera_, player_);
	}

	// 障害物情報の読み込み
	LoadStage("Resources/stage1/stage1.obj");

	// EnemyLoaderの生成と初期化
	enemyLoader_ = new EnemyLoader();
	std::string enemiesFile = "Resources/enemies" + std::to_string(currentStage_) + ".csv";
	// CSVから敵の情報を読み込み
	if (enemyLoader_->LoadEnemyData(enemiesFile)) {
		// 敵を生成
		enemyLoader_->CreateEnemies(&camera_, player_, allObstacles_);
	}

	// 各種敵リストをプレイヤーに設定
	player_->SetEnemyList(enemyLoader_->GetEnemyList());

	// キャノン敵への参照をプレイヤーに設定
	if (!enemyLoader_->GetCannonEnemyList().empty()) {
		player_->SetCannon(enemyLoader_->GetCannonEnemyList()[0]); // 一番最初のキャノン敵を設定
	}

	// バネ敵への参照をプレイヤーに設定
	player_->SetSpringEnemies(enemyLoader_->GetSpringEnemyList());

	// ブロックへの参照をプレイヤーに設定
	player_->SetBlock(block_);
}

void GameScene::Update() {
	player_->Update();

	// EnemyLoaderの更新
	if (enemyLoader_) {
		enemyLoader_->Update();
	}

	// MapLoaderの更新
	if (mapLoader_) {
		mapLoader_->Update();
	}

	block_->Update();
	player_->DrawUI();
	skydome_->Update();

	// 　↓　ゴールしたら1と2ステージループするようになってる、切り替え処理2を消すとステージ3に進む

	// ステージ切り替え処理
	if (currentStage_ == 1) {
		if (mapLoader_ && mapLoader_->IsDoorOpened()) {
			ChangeStage(currentStage_ + 1);
		}
	}

	// ステージ切り替え処理2
	if (currentStage_ == 2) {
		if (mapLoader_ && mapLoader_->IsDoorOpened()) {
			ChangeStage(currentStage_ - 1);
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

	stage->Draw(worldTransform_, camera_, textureHandle);

	player_->Draw();

	// EnemyLoaderで読み込んだ敵の描画
	if (enemyLoader_) {
		enemyLoader_->Draw();
	}

	// MapLoaderで読み込んだオブジェクト（鍵とドア）の描画
	if (mapLoader_) {
		mapLoader_->Draw();
	}

	block_->Draw();
	skydome_->Draw();

	Model::PostDraw();

	// UI描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();
}

void GameScene::ChangeStage(int nextStage) {
	currentStage_ = nextStage;

	stage = Model::CreateFromOBJ("stage" + std::to_string(currentStage_), true);

	// 新しいオブジェクトデータをロード
	if (mapLoader_) {
		std::string objectsFile = "Resources/objects" + std::to_string(currentStage_) + ".csv";
		mapLoader_->ChangeStage(currentStage_, &camera_, player_);

		if (mapLoader_->LoadMapData(objectsFile)) {
			mapLoader_->CreateObjects(&camera_, player_);
		}
	}

	// プレイヤーの既存の障害物リストをクリア
	player_->ClearObstacleList();

	// プレイヤーに新しい障害物リストを設定
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	// ブロックへの参照を再設定
	player_->SetBlock(block_);

	// EnemyLoaderをリセット
	if (enemyLoader_) {
		// プレイヤーの敵リストをクリア
		player_->SetEnemyList({});
		player_->SetSpringEnemies({});

		// player_->SetCannon(nullptr);
		// delete enemyLoader_;

		enemyLoader_ = new EnemyLoader();

		// ステージごとの敵配置を読み込み
		std::string enemiesFile = "Resources/enemies" + std::to_string(currentStage_) + ".csv";
		if (enemyLoader_->LoadEnemyData(enemiesFile)) {
			enemyLoader_->CreateEnemies(&camera_, player_, allObstacles_);
		}

		// 各種敵リストをプレイヤーに設定
		player_->SetEnemyList(enemyLoader_->GetEnemyList());

		// キャノン敵への参照をプレイヤーに設定
		if (!enemyLoader_->GetCannonEnemyList().empty()) {
			player_->SetCannon(enemyLoader_->GetCannonEnemyList()[0]);
		}

		// バネ敵への参照をプレイヤーに設定
		player_->SetSpringEnemies(enemyLoader_->GetSpringEnemyList());
	}

	std::string stageFile = "Resources/stage" + std::to_string(currentStage_) + "/stage" + std::to_string(currentStage_) + ".obj";
	LoadStage(stageFile);
}

// AddObstacle、LoadStage、UpdateStageAABBメソッドはそのまま以前の実装を使用
void GameScene::AddObstacle(std::vector<std::vector<AABB>>& allObstacles, const Vector3& min, const Vector3& max) {
	AABB obstacle;
	obstacle.min = min;
	obstacle.max = max;
	if (allObstacles.empty() || allObstacles.back().size() >= 100) { // 100個の障害物を追加
		allObstacles.emplace_back();
	}
	allObstacles.back().push_back(obstacle);
}

void GameScene::LoadStage(std::string objFile) {
	// ステージデータの読み込み
	std::ifstream file;
	file.open(objFile);
	assert(file.is_open());

	// Commandをリセット
	Command.str("");
	Command.clear();

	Command << file.rdbuf();
	file.close();

	// 障害物データをクリア
	allObstacles_.clear();

	UpdateStageAABB();

	player_->ClearObstacleList(); // 古い障害物リストを削除
	// 新しい障害物リスト
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}
}

void GameScene::UpdateStageAABB() {
	std::string line;
	uint32_t cornerNumber = 0;

	Vector3 max;
	Vector3 min;

	// AABB stageAABB;
	bool start = false;
	bool reverse = false;

	while (getline(Command, line)) {
		std::istringstream line_stream(line);

		std::string word;

		getline(line_stream, word, ' ');

		if (word.find("v") == 0) {
			cornerNumber++;
		} else if (word.find("vn") == 0) {
			break;
		} else {
			continue;
		}

		if (cornerNumber > 0) {

			getline(line_stream, word, ' ');
			float x = (float)std::atof(word.c_str());

			getline(line_stream, word, ' ');
			float y = (float)std::atof(word.c_str());

			getline(line_stream, word, ' ');
			float z = (float)std::atof(word.c_str());

			if (!start) {
				max = {x, y, z};
				min = {x, y, z};
				start = true;
			} else {

				// 前よりも大きいとき
				if (max.x <= x) {
					max.x = x;
				}
				// 前よりも小さいとき
				if (min.x > x) {
					min.x = x;
				}

				if (max.y <= y) {
					max.y = y;
				}

				if (min.y > y) {
					min.y = y;
				}

				if (max.z <= z) {
					max.z = z;
				}

				if (min.z > z) {
					min.z = z;
				}
			}
		}

		if (cornerNumber == 8) {
			if (!reverse) {
				AddObstacle(allObstacles_, min, max); // 結合した基盤となるobj
			} else {

				float minX;
				float maxX;
				maxX = -(max.x);
				minX = -(min.x);

				min.x = maxX;
				max.x = minX;
				AddObstacle(allObstacles_, {min.x, min.y, min.z}, {max.x, max.y, max.z}); // それ以外のすべてobj
			}

			cornerNumber = 0;
			start = false;
			reverse = true;
		}
	}
}