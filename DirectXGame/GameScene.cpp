#include "GameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete player_;
	delete modelPlayer_;
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

}

void GameScene::Update() { player_->Update(); }

void GameScene::Draw() { 
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//背景
	Sprite::PreDraw(commandList);



	Sprite::PostDraw();

	//モデル
	Model::PreDraw(commandList);
	player_->Draw();

	Model::PostDraw();

	
	//UI
	Sprite::PreDraw(commandList);

	Sprite::PostDraw();
}