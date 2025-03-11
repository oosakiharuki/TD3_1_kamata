#include "3d\WorldTransform.h"
#include "Mymath.h"

void WorldTransform::UpdateMatrix() {
	// スケール、回転、平行移動を合成して行列を計算する
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	//親子関係
	if (parent_) {
		matWorld_ = MatrixMultiply(matWorld_, parent_->matWorld_);
	}

	// 定数バッファに転送する
	TransferMatrix();
}