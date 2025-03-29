#include "Block.h"

Block::Block() {}

Block::~Block() { delete model_; }

void Block::Init(Camera* camera_) {
	viewProjection_ = camera_;

	// worldTransformを確実に初期化
	worldTransform.Initialize();

	// モデルの読み込み
	model_ = Model::CreateFromOBJ("cube", true);
	// 行列の更新
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatrix();
}

void Block::Update() {
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatrix();
}

void Block::Draw() {
	// 条件式の修正: !isActive_ ではなく isActive_ のときに描画
	if (isActive_) {
		model_->Draw(worldTransform, *viewProjection_);
	}
	// 非アクティブなら描画しない (ここでreturnを入れるなら上のifブロックの外に出す)
}

AABB Block::GetAABB() const {
	AABB aabb;
	aabb.min = worldTransform.translation_ - Vector3(12.0f, 1.0f, 2.0f);
	aabb.max = worldTransform.translation_ + Vector3(12.0f, 15.0f, 2.0f);
	return aabb;
}

void Block::SetPosition(const Vector3& pos) {
	worldTransform.translation_ = pos;
	worldTransform.UpdateMatrix();
}