#include "StageManager.h"
#include "imgui.h"
#include <algorithm>
#include <fstream>

StageManager::StageManager() {}

StageManager::~StageManager() {
	// リソースの解放
	if (stageModel_)
		delete stageModel_;
	if (ground_)
		delete ground_;
	if (key_)
		delete key_;
	if (door_)
		delete door_;
	if (cannonEnemy_)
		delete cannonEnemy_;

	// 敵の解放
	for (auto enemy : enemyList_) {
		delete enemy;
	}
	enemyList_.clear();
}

void StageManager::Initialize(Camera* camera, uint32_t textureHandle) {
	camera_ = camera;
	textureHandle_ = textureHandle;

	// ステージモデルの読み込み
	stageModel_ = Model::CreateFromOBJ("stage", true);

	// 第1ステージの初期化
	InitializeStage(1);
}

void StageManager::InitializeStage(int stageNumber) {
	// ステージ番号を設定
	currentStage_ = stageNumber;

	// ステージ情報のリセット
	allObstacles_.clear();
	stageCommand_.str("");
	stageCommand_.clear();

	// ステージの地形をロード
	LoadStage("Resources/stage/stage.obj");
	UpdateStageAABB();

	// プレイヤーの位置をリセット
	ResetPlayerPosition(stageNumber);

	// 既存の敵を削除
	for (auto enemy : enemyList_) {
		delete enemy;
	}
	enemyList_.clear();

	// ステージごとの敵配置
	if (stageNumber == 1) {
		// 第1ステージの敵配置
		for (int i = 0; i < 5; ++i) {
			Enemy* enemy = new Enemy();
			enemy->Init(camera_);
			for (const auto& obstacles : allObstacles_) {
				enemy->SetObstacleList(obstacles);
			}
			enemyList_.push_back(enemy);
		}

		// 各Enemyの初期位置を設定
		if (enemyList_.size() > 0)
			enemyList_[0]->SetPosition({-20.0f, 10.0f, -10.0f});
		if (enemyList_.size() > 1)
			enemyList_[1]->SetPosition({-10.0f, 10.0f, -10.0f});
		if (enemyList_.size() > 2)
			enemyList_[2]->SetPosition({20.0f, 10.0f, -20.0f});
		if (enemyList_.size() > 3)
			enemyList_[3]->SetPosition({-40.0f, 10.0f, -10.0f});
		if (enemyList_.size() > 4)
			enemyList_[4]->SetPosition({50.0f, 10.0f, -20.0f});
	} else if (stageNumber == 2) {
		// 第2ステージの敵配置（位置を変更）
		for (int i = 0; i < 5; ++i) {
			Enemy* enemy = new Enemy();
			enemy->Init(camera_);
			for (const auto& obstacles : allObstacles_) {
				enemy->SetObstacleList(obstacles);
			}
			enemyList_.push_back(enemy);
		}

		// 第2ステージでは敵の位置を変更
		if (enemyList_.size() > 0)
			enemyList_[0]->SetPosition({15.0f, 10.0f, -15.0f});
		if (enemyList_.size() > 1)
			enemyList_[1]->SetPosition({-25.0f, 10.0f, -15.0f});
		if (enemyList_.size() > 2)
			enemyList_[2]->SetPosition({0.0f, 10.0f, -30.0f});
		if (enemyList_.size() > 3)
			enemyList_[3]->SetPosition({-20.0f, 10.0f, -40.0f});
		if (enemyList_.size() > 4)
			enemyList_[4]->SetPosition({20.0f, 10.0f, -40.0f});
	}

	// CannonEnemyの再初期化
	if (cannonEnemy_) {
		delete cannonEnemy_;
	}
	cannonEnemy_ = new CannonEnemy();
	cannonEnemy_->Init(camera_);
	for (const auto& obstacles : allObstacles_) {
		cannonEnemy_->SetObstacleList(obstacles);
	}
	cannonEnemy_->SetPlayer(player_);

	// プレイヤーの更新
	if (player_) {
		// 障害物リストを更新
		for (const auto& obstacles : allObstacles_) {
			player_->SetObstacleList(obstacles);
		}
		player_->SetEnemyList(enemyList_);
		player_->SetCannon(cannonEnemy_);
	}

	// Groundの再初期化
	if (ground_) {
		delete ground_;
	}
	ground_ = new Ground();
	ground_->Init(camera_);

	// 鍵とドアの初期化（第1ステージのみ）
	if (stageNumber == 1) {
		// 鍵の初期化
		if (key_) {
			delete key_;
		}
		key_ = new Key();
		key_->Init(camera_);
		key_->SetPlayer(player_);

		// ドアの初期化
		if (door_) {
			delete door_;
		}
		door_ = new Door();
		door_->Init(camera_);
		door_->SetPlayer(player_);
		door_->SetKey(key_);
	} else {
		// 第2ステージでは鍵とドアを削除
		if (key_) {
			delete key_;
			key_ = nullptr;
		}
		if (door_) {
			delete door_;
			door_ = nullptr;
		}
	}
}

void StageManager::TransitionToStage(int stageNumber) {
	if (!isStageTransitioning_) {
		isStageTransitioning_ = true;
		transitionTimer_ = 0.0f;

		// 新しいステージの初期化
		InitializeStage(stageNumber);
	}
}

void StageManager::Update() {
	// ステージ遷移処理
	if (isStageTransitioning_) {
		transitionTimer_ += 1.0f / 60.0f; // 60FPSを想定

		if (transitionTimer_ >= transitionDuration_) {
			isStageTransitioning_ = false;
		}
		return;
	}

	// ステージ固有の更新処理
	if (currentStage_ == 1) {
		// 第1ステージ固有の処理
		if (key_)
			key_->Update();
		if (door_)
			door_->Update();

		// ドアが開いた状態でプレイヤーがドアに触れたら第2ステージへ
		if (door_ && door_->IsDoorOpened() && door_->IsDoorTouched()) {
			TransitionToStage(2);
		}
	} else if (currentStage_ == 2) {
		// 第2ステージ固有の処理
		// 特に何もない場合は空欄
	}

	// 敵の更新
	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		(*it)->Update();
		if (player_ && std::find(player_->enemyList_.begin(), player_->enemyList_.end(), *it) == player_->enemyList_.end()) {
			delete *it;
			it = enemyList_.erase(it);
		} else {
			++it;
		}
	}

	// キャノン敵の更新
	if (cannonEnemy_) {
		cannonEnemy_->Update();
	}

	// ImGuiでステージ情報を表示
	ImGui::Begin("Stage Info");
	ImGui::Text("Current Stage: %d", currentStage_);
	if (currentStage_ == 1) {
		ImGui::Text("Mission: Find the key and open the door");
		if (key_ && key_->IsKeyObtained()) {
			ImGui::Text("Key Status: Obtained!");
		} else {
			ImGui::Text("Key Status: Not found yet");
		}

		if (door_ && door_->IsDoorOpened()) {
			ImGui::Text("Door Status: Opened!");
		} else {
			ImGui::Text("Door Status: Closed");
		}
	} else if (currentStage_ == 2) {
		ImGui::Text("Mission: Explore Stage 2");
		ImGui::Text("Congratulations on clearing Stage 1!");
	}
	ImGui::End();
}

void StageManager::Draw() {
	// ステージ遷移中は描画しない
	if (isStageTransitioning_) {
		return;
	}

	// ステージモデルの描画
	if (stageModel_) {
		WorldTransform worldTransform;
		worldTransform.Initialize();
		stageModel_->Draw(worldTransform, *camera_, textureHandle_);
	}

	// 敵の描画
	for (auto enemy : enemyList_) {
		enemy->Draw();
	}

	// キャノン敵の描画
	if (cannonEnemy_) {
		cannonEnemy_->Draw();
	}

	// ステージ固有の描画
	if (currentStage_ == 1) {
		// 第1ステージ固有の描画
		if (key_)
			key_->Draw();
		if (door_)
			door_->Draw();
	}

	// グラウンドの描画
	if (ground_) {
		ground_->Draw();
	}
}

void StageManager::LoadStage(const std::string& objFile) {
	std::ifstream file;
	file.open(objFile);
	assert(file.is_open());

	stageCommand_ << file.rdbuf();

	file.close();
}

void StageManager::UpdateStageAABB() {
	std::string line;
	uint32_t cornerNumber = 0;

	Vector3 max;
	Vector3 min;

	bool start = false;
	bool reverse = false;

	while (getline(stageCommand_, line)) {
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
				if (max.x <= x)
					max.x = x;
				if (min.x > x)
					min.x = x;
				if (max.y <= y)
					max.y = y;
				if (min.y > y)
					min.y = y;
				if (max.z <= z)
					max.z = z;
				if (min.z > z)
					min.z = z;
			}
		}

		if (cornerNumber == 8) {
			if (!reverse) {
				AddObstacle(min, max);
			} else {
				float minX = -(max.x);
				float maxX = -(min.x);
				min.x = minX;
				max.x = maxX;
				AddObstacle(min, max);
			}

			cornerNumber = 0;
			start = false;
			reverse = true;
		}
	}
}

void StageManager::AddObstacle(const Vector3& min, const Vector3& max) {
	AABB obstacle;
	obstacle.min = min;
	obstacle.max = max;

	if (allObstacles_.empty() || allObstacles_.back().size() >= 100) {
		allObstacles_.emplace_back();
	}
	allObstacles_.back().push_back(obstacle);
}

void StageManager::ResetPlayerPosition(int stageNumber) {
	if (!player_)
		return;

	if (stageNumber == 1) {
		// 第1ステージのプレイヤー初期位置
		player_->SetPosition({0.0f, 10.0f, -10.0f});
	} else if (stageNumber == 2) {
		// 第2ステージのプレイヤー初期位置
		player_->SetPosition({0.0f, 10.0f, -40.0f});
	}
}