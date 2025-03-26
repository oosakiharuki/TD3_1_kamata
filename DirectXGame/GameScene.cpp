#include "GameScene.h"
#include "AABB.h"
#include <cassert>
#include <fstream>
#include <vector>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;

	// MapLoaderの解放
	delete mapLoader_;

	// EnemyLoaderの解放
	delete enemyLoader_;

	delete stage;
	delete stage2; // ステージ2のモデルも解放
	delete block_;
	delete modelBlock_;

	delete skydome_;
	delete modelSkydome_;
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	camera_.Initialize();

	
	block_ = new Block();
	modelBlock_ = Model::Create();
	block_->Init(modelBlock_, &camera_);

	// Player の生成と初期化
	player_ = new Player();
	player_->Init(&camera_);

	// ステージ1のモデルを読み込み
	stage = Model::CreateFromOBJ("stage", true);

	// ステージ2のモデルも事前に読み込み（まだ使わない）
	stage2 = Model::CreateFromOBJ("stage2", true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("space", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// 最初はステージ1の障害物情報を読み込み
	LoadStage("Resources/stage/stage.obj");
	UpdateStageAABB();

	// MapLoaderの生成と初期化
	mapLoader_ = new MapLoader();
	// CSVからマップオブジェクト（鍵とドア）を読み込み
	if (mapLoader_->LoadMapData("Resources/objects.csv")) {
		// オブジェクトを生成
		mapLoader_->CreateObjects(&camera_, player_);
	}

	// EnemyLoaderの生成と初期化
	enemyLoader_ = new EnemyLoader();
	// CSVから敵の情報を読み込み
	if (enemyLoader_->LoadEnemyData("Resources/enemies.csv")) {
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

	// 障害物リストを Player にセット
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	// ステージ遷移タイマーの初期化
	stageTransitionTimer = 5.0f;
	currentStage = 1;
	isTransitioning = false;
}

void GameScene::Update() {
	// ステージ遷移タイマー処理
	if (currentStage == 1) {
		stageTransitionTimer -= 1.0f / 60.0f; // 60FPSを想定

		if (stageTransitionTimer <= 0 && !isTransitioning) {
			isTransitioning = true;
			TransitionToStage2();
		}
	}

	// 元々の更新処理
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

	#ifdef _DEBUG
	ImGui::Begin("Stage Info");
	ImGui::Text("Current Stage: %d", currentStage);
	if (currentStage == 1) {
		ImGui::Text("Stage Transition in: %.1f seconds", stageTransitionTimer);
	}
	ImGui::End();
#endif
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	// モデル描画
	Model::PreDraw(commandList);

	// 現在のステージに応じたステージモデルを描画
	if (currentStage == 1) {
		stage->Draw(worldTransform_, camera_, textureHandle);
	} else if (currentStage == 2) {
		stage2->Draw(worldTransform_, camera_, textureHandle);
	}

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
	std::ifstream file;
	file.open(objFile);
	assert(file.is_open());

	Command << file.rdbuf();

	file.close();
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

// ステージ2への遷移処理
void GameScene::TransitionToStage2() {
	// 1. 古い当たり判定をクリア
	allObstacles_.clear();
	Command.str("");
	Command.clear();

	// 2. Stage2を読み込み
	LoadStage("Resources/stage2/stage2.obj");
	UpdateStageAABB();

	// 3. 敵とオブジェクトの再配置
	// MapLoaderをリセット
	if (mapLoader_) {
		delete mapLoader_;
		mapLoader_ = new MapLoader();

		// Stage2用のCSVを読み込み（ファイル名は適宜変更）
		if (mapLoader_->LoadMapData("Resources/objects.csv")) {
			mapLoader_->CreateObjects(&camera_, player_);
		}
	}

	// EnemyLoaderをリセット
	if (enemyLoader_) {
		delete enemyLoader_;
		enemyLoader_ = new EnemyLoader();

		// Stage2用の敵配置を読み込み（ファイル名は適宜変更）
		if (enemyLoader_->LoadEnemyData("Resources/enemies2.csv")) {
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

	// プレイヤーの既存の障害物リストをクリア
	player_->ClearObstacleList();

	// プレイヤーに新しい障害物リストを設定
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	// ブロックへの参照を再設定
	player_->SetBlock(block_);

	// ステージ番号を更新
	currentStage = 2;
	isTransitioning = false;

	// プレイヤー位置のリセット（必要に応じて）
	// player_->Reset();
}