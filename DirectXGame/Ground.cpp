#include "Ground.h"
#include <cmath>

Ground::Ground() {}

Ground::~Ground() { delete groundModel_; }

void Ground::Init(Camera* camera) {
	camera_ = camera;
	worldTransform.Initialize();
	// "ground" という名前のOBJファイルを読み込み
	groundModel_ = Model::CreateFromOBJ("ground", true);
}

void Ground::Update() { worldTransform.TransferMatrix(); }

void Ground::Draw() { groundModel_->Draw(worldTransform, *camera_); }

float Ground::GetHeightAt(const Vector3& pos) {
	// サンプル実装：
	// 実際にはgroundModel_の三角形データとレイとの交差判定を行うのが望ましいですが、
	// ここでは簡易的にx,z座標に基づく凹凸をシミュレートしています。
	return sin(pos.x * 0.1f) * 2.0f + cos(pos.z * 0.1f) * 2.0f;
}
