#include "GameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player_;
	delete modelPlayer_;
	delete block_;
	delete modelBlock_;
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	viewProjection_.Initialize();

	modelPlayer_ = Model::Create();
	player_ = new Player();
	Vector3 playerPos(-5, 0, 0);
	player_->Init(modelPlayer_, &viewProjection_, playerPos);

	modelBlock_ = Model::Create();
	block_ = new Block();
	block_->Init(modelBlock_, &viewProjection_);
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
	block_->Draw();

	Model::PostDraw();


	//UI
	Sprite::PreDraw(commandList);

	Sprite::PostDraw();
}