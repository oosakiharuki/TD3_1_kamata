#include "GameScene.h"
#include "AABB.h"
#include <vector>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;
	delete modelGround_;
	for (auto enemy : enemyList_) {
		delete enemy;
	}
	delete cannonEenmy;
	delete stage;
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
	player_->Init(&camera_,textureHandle);

	// 障害物リストの作成例
	//AddObstacle(allObstacles_, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f});            // 例：壁のAABB
	//AddObstacle(allObstacles_, {-200.0f, -5.5f, -200.0f}, {200.0f, 0.0f, 200.0f}); // 例：床のAABB全体の床
	//AddObstacle(allObstacles_, {-10.0f, -0.5f, -10.0f}, {20.0f, 3.0f, 20.0f});     // 新しい足場AABB白
	//AddObstacle(allObstacles_, {18.0f, -0.5f, -10.0f}, {37.5f, 6.0f, 20.0f});      // 新しい足場のAABB白

	//AddObstacle(allObstacles_, {6.2f, -0.5f, -41.0f}, {19.5f, 3.5f, -27.3f});  //宇宙模様の床

	stage = Model::CreateFromOBJ("stage",true);

	//AddObstacle(allObstacles_, { -100.0f, - 1.0f, - 100.0f }, { 100.0f, 1.0f, 100.0f }); // 宇宙模様の床
	//AddObstacle(allObstacles_, {-29.9f, 1.0f, -39.9f}, { -49.9f, 3.0f, - 19.9f}); // 宇宙模様の床

	LoadStage("Resources/stage/stage.obj");
	UpdateStageAABB();

	    // Enemyの生成と初期化
	for (int i = 0; i < 5; ++i) { // 例として5体のEnemyを生成
		Enemy* enemy = new Enemy();
		enemy->Init(&camera_);
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
	
	cannonEenmy = new CannonEnemy();
	cannonEenmy->Init(&camera_);
	for (const auto& obstacles : allObstacles_) {
		cannonEenmy->SetObstacleList(obstacles);
	}
	cannonEenmy->SetPlayer(player_);

	player_->SetEnemyList(enemyList_);


	// 障害物リストを Player にセット
	for (const auto& obstacles : allObstacles_) {
		player_->SetObstacleList(obstacles);
	}

	player_->SetCannon(cannonEenmy);

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
			++it;
		}
	}

	//cannonEenmy->SetPlayerAABB(player_->GetAABB());
	cannonEenmy->Update();

	/*/
	for (auto enemy : enemyList_) {
		enemy->Update();
	}
	/*/
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	// モデル描画
	Model::PreDraw(commandList);

	stage->Draw(worldTransform_,camera_,textureHandle);



	player_->Draw();
	//modelGround_->Draw();
	for (auto enemy : enemyList_) {
		enemy->Draw();
	}

	cannonEenmy->Draw();

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

	//AABB stageAABB;
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
		//2と7
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
				AddObstacle(allObstacles_, min, max); // 宇宙模様の床
			} else {

				float minX;
				float maxX;
				maxX = -(max.x);
				minX = -(min.x);

				min.x = maxX;
				max.x = minX;
				AddObstacle(allObstacles_, {min.x, min.y, min.z}, {max.x, max.y, max.z}); // 宇宙模様の床
			}
			
			cornerNumber = 0;
			start = false;
			reverse = true;
		}
	}
}
