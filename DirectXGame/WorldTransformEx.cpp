#include "MyMath.h"
#include "3d/WorldTransform.h"
using namespace KamataEngine;
using namespace MyMath;

void WorldTransform::UpdateMatrix() {

	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}

	TransferMatrix();
}