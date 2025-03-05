//#include "Ground.h"
//
//void Ground::Initialize(Camera* camera) {
//	worldTransform_.Initialize();
//	camera_ = camera;
//	worldTransform_.translation_.y = -1.9f;
//	worldTransform_.translation_.z = 0.0f; // 初期位置
//	groundModel_ = Model::CreateFromOBJ("ground", true);
//}
//
//void Ground::Update() {
//	// 行列を定数バッファに転送
//	worldTransform_.TransferMatrix();
//
//	// Z軸に沿って地面を動かす
//	worldTransform_.translation_.z -= 0.6f; // スクロール速度を設定
//
//	// 地面が一定のZ位置を超えたらリセットする
//	if (worldTransform_.translation_.z < -113.0f) {
//		worldTransform_.translation_.z = worldTransform_.translation_.z + 226.0f; // Ground2 の終わりと連携して位置を調整
//	}
//
//	worldTransform_.UpdateMatrix();
//}
//
//void Ground::Draw() {
//	// 3Dモデル描画
//	groundModel_->Draw(worldTransform_, *camera_);
//}
