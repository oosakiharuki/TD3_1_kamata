#include "Ground.h"

void Ground::Init(Camera* camera, Model* model) {
	camera_ = camera;
	worldTransform.Initialize();
	model_ = model;
	//groundModel_ = Model::CreateFromOBJ("ground", true);
}

void Ground::Update() {}

void Ground::Draw() { model_->Draw(worldTransform, *camera_); }

