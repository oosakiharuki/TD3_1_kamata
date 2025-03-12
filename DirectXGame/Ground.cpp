#include "Ground.h"

void Ground::Init(Camera* camera) {
	camera_ = camera;
	worldTransform.Initialize();
	// "ground" という名前のOBJファイルを読み込みます。
	groundModel_ = Model::CreateFromOBJ("ground", true);
}

void Ground::Update() { worldTransform.TransferMatrix(); }

void Ground::Draw() { groundModel_->Draw(worldTransform, *camera_); }