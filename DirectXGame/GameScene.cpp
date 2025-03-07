#include "GameScene.h"
#include "AABB.h"
#include <vector>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;

	delete modelPlayer_;

	delete test;
	delete modelT;


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
	std::vector<AABB> obstacles1;
	std::vector<AABB> obstacles2;
	std::vector<AABB> obstacles3;
	std::vector<AABB> obstacles4;

	// 例：壁のAABB（仮の座標・サイズ）
	AABB wall;
	wall.min = {0.0f, 0.0f, 0.0f};
	wall.max = {0.0f, 0.0f, 0.0f};
	obstacles1.push_back(wall);

	// 例：床のAABB
	AABB floor;
	floor.min = {-200.0f, -5.5f, -200.0f};
	floor.max = {200.0f, 0.0f, 200.0f};
	obstacles2.push_back(floor);


	modelT = Model::Create();
	test = new Test();
	test->Init(modelT, &viewProjection_);


	// 新しい足場AABB
	AABB newPlatform;
	newPlatform.min = {-10.0f, -0.5f, -10.0f}; // 新しい足場の位置とサイズを設定
	newPlatform.max = {20.0f, 3.0f, 20.0f}; // 新しい足場の位置とサイズを設定
	obstacles3.push_back(newPlatform);

		// 新しい足場のAABB
	AABB newPlatform2;
	newPlatform2.min = {18.0f, -0.5f, -10.0f}; // 新しい足場の位置とサイズを設定
	newPlatform2.max = {37.5f, 6.0f, 20.0f};    // 新しい足場の位置とサイズを設定
	obstacles4.push_back(newPlatform2);

	// 障害物リストを Player にセット
	player_->SetObstacleList(obstacles1);
	player_->SetObstacleList(obstacles2);
	player_->SetObstacleList(obstacles3);
	player_->SetObstacleList(obstacles4);

	// Ground の生成・初期化
	modelGround_ = new Ground();
	modelGround_->Init(&camera_);
}



//void GameScene::Update() { player_->Update(); }

void GameScene::Update() { 
	player_->Update();
	test->Update();

	Collision();
	
	ImGui::Begin("camera");
	ImGui::DragFloat3("cameraTranslate", &viewProjection_.translation_.x,0.1f);
	ImGui::DragFloat3("cameraRotate", &viewProjection_.rotation_.x,0.1f);
	ImGui::End();

	viewProjection_.UpdateMatrix();
}


void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	// モデル描画
	Model::PreDraw(commandList);
	player_->Draw();

	test->Draw();


	modelGround_->Draw();

	Model::PostDraw();

	// UI描画
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();
}

void GameScene::Collision() {

	if ((player_->GetAABB().max.x >= test->GetAABB().min.x && player_->GetAABB().min.x <= test->GetAABB().max.x) &&
	    (player_->GetAABB().max.y >= test->GetAABB().min.y && player_->GetAABB().min.y - 1.0f <= test->GetAABB().max.y) &&
	    (player_->GetAABB().max.z >= test->GetAABB().min.z && player_->GetAABB().min.z <= test->GetAABB().max.z)) {
		

		//頭の上か
		if (player_->GetAABB().min.y >= test->GetAABB().max.y - 1.5f) {

			player_->IsOnEnemy(true);
			player_->GetHead(test->GetWorldTranslate().y + 2);

			if (player_->GetIsTransfar()) {
				test->ContralPlayer();
				player_->SetEnemyContral(true);
			}
		}
	} 
	else {
		player_->IsOnEnemy(false);
		if (!player_->GetEnemyContral()) {
			player_->GetHead(0); // 地面のy座標
		}
	}

	if (player_->GetEnemyContral()) {
		test->SetParent(player_->GetWorld());
	} else {		
		test->ReMove(player_->GetWorld()->translation_);
	}

}
