#include "GameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete player_;
	delete modelPlayer_;

	delete test;
	delete modelT;

}

void GameScene::Initialize() { 
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	viewProjection_.Initialize();
	
	modelPlayer_ = Model::Create();
	player_ = new Player();
	player_->Init(modelPlayer_, &viewProjection_);

	modelT = Model::Create();
	test = new Test();
	test->Init(modelT, &viewProjection_);

}

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

	//背景
	Sprite::PreDraw(commandList);



	Sprite::PostDraw();

	//モデル
	Model::PreDraw(commandList);
	player_->Draw();
	test->Draw();

	Model::PostDraw();

	
	//UI
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
